#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>


#define PORT 8090

#define BUFFER_SIZE 16384
#define PATH_SIZE   1024

#define DOSSIER_IHM "/home/ecrin/AP/IHM/"


// ============================================================
// ENVOYER TOUTES LES DONNEES
// ============================================================

int envoyer_tout(int client_fd, const char *data, size_t length)
{
    size_t total = 0;

    while (total < length)
    {
        ssize_t n = send(
            client_fd,
            data + total,
            length - total,
            0
        );

        if (n <= 0)
        {
            return -1;
        }

        total += (size_t)n;
    }

    return 0;
}


// ============================================================
// LIRE UN FICHIER COMPLET
// ============================================================

char *lire_fichier(const char *chemin, size_t *taille)
{
    FILE *fichier = fopen(chemin, "rb");

    if (fichier == NULL)
    {
        perror(chemin);
        return NULL;
    }

    fseek(fichier, 0, SEEK_END);

    long longueur = ftell(fichier);

    if (longueur < 0)
    {
        fclose(fichier);
        return NULL;
    }

    rewind(fichier);


    char *contenu = malloc((size_t)longueur + 1);

    if (contenu == NULL)
    {
        fclose(fichier);
        return NULL;
    }


    size_t lu = fread(
        contenu,
        1,
        (size_t)longueur,
        fichier
    );


    contenu[lu] = '\0';

    fclose(fichier);


    if (taille != NULL)
    {
        *taille = lu;
    }


    return contenu;
}


// ============================================================
// ENVOYER LES HEADERS HTTP
// ============================================================

void envoyer_headers(
    int client_fd,
    const char *status,
    const char *content_type,
    size_t content_length
)
{
    char headers[2048];


    int n = snprintf(
        headers,
        sizeof(headers),

        "HTTP/1.1 %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "Connection: close\r\n"
        "\r\n",

        status,
        content_type,
        content_length
    );


    if (n > 0)
    {
        envoyer_tout(
            client_fd,
            headers,
            (size_t)n
        );
    }
}


// ============================================================
// SERVIR UN FICHIER
// ============================================================

void servir_fichier(
    int client_fd,
    const char *nom,
    const char *content_type
)
{
    char chemin[PATH_SIZE];


    snprintf(
        chemin,
        sizeof(chemin),
        "%s%s",
        DOSSIER_IHM,
        nom
    );


    size_t taille = 0;

    char *contenu =
        lire_fichier(
            chemin,
            &taille
        );


    if (contenu == NULL)
    {
        const char *erreur =
            "404 - Fichier introuvable";


        envoyer_headers(
            client_fd,
            "404 Not Found",
            "text/plain; charset=utf-8",
            strlen(erreur)
        );


        envoyer_tout(
            client_fd,
            erreur,
            strlen(erreur)
        );

        return;
    }


    envoyer_headers(
        client_fd,
        "200 OK",
        content_type,
        taille
    );


    envoyer_tout(
        client_fd,
        contenu,
        taille
    );


    free(contenu);
}


// ============================================================
// ECHAPPER UNE CHAINE POUR JSON
// ============================================================

char *json_escape(const char *texte)
{
    if (texte == NULL)
    {
        return NULL;
    }


    size_t longueur = strlen(texte);


    /*
        On réserve large :
        certains caractères peuvent prendre 2 caractères
        après échappement.
    */

    char *resultat =
        malloc(longueur * 2 + 1);


    if (resultat == NULL)
    {
        return NULL;
    }


    size_t j = 0;


    for (size_t i = 0; i < longueur; i++)
    {
        char c = texte[i];


        switch (c)
        {
            case '"':
                resultat[j++] = '\\';
                resultat[j++] = '"';
                break;


            case '\\':
                resultat[j++] = '\\';
                resultat[j++] = '\\';
                break;


            case '\n':
                resultat[j++] = '\\';
                resultat[j++] = 'n';
                break;


            case '\r':
                resultat[j++] = '\\';
                resultat[j++] = 'r';
                break;


            case '\t':
                resultat[j++] = '\\';
                resultat[j++] = 't';
                break;


            default:
                resultat[j++] = c;
                break;
        }
    }


    resultat[j] = '\0';


    return resultat;
}


// ============================================================
// EXTRAIRE "commande" DU JSON
//
// Exemple :
//
// {"commande":"hostname"}
// ============================================================

int extraire_commande(
    const char *json,
    char *commande,
    size_t taille_commande
)
{
    const char *position =
        strstr(
            json,
            "\"commande\""
        );


    if (position == NULL)
    {
        return 0;
    }


    position = strchr(
        position,
        ':'
    );


    if (position == NULL)
    {
        return 0;
    }


    position++;


    /*
        Chercher le premier guillemet
    */

    position = strchr(
        position,
        '"'
    );


    if (position == NULL)
    {
        return 0;
    }


    position++;


    const char *fin =
        strchr(
            position,
            '"'
        );


    if (fin == NULL)
    {
        return 0;
    }


    size_t longueur =
        (size_t)(fin - position);


    if (longueur >= taille_commande)
    {
        longueur =
            taille_commande - 1;
    }


    memcpy(
        commande,
        position,
        longueur
    );


    commande[longueur] = '\0';


    return 1;
}


// ============================================================
// EXECUTER UNE COMMANDE
// ============================================================

char *executer_commande(const char *commande)
{
    /*
        Attention :
        cette fonction exécute réellement une commande shell.
    */

    char commande_complete[4096];


    snprintf(
        commande_complete,
        sizeof(commande_complete),
        "%s 2>&1",
        commande
    );


    FILE *pipe =
        popen(
            commande_complete,
            "r"
        );


    if (pipe == NULL)
    {
        return strdup(
            "Impossible d'executer la commande"
        );
    }


    size_t capacite = 8192;
    size_t longueur = 0;


    char *resultat =
        malloc(capacite);


    if (resultat == NULL)
    {
        pclose(pipe);
        return NULL;
    }


    resultat[0] = '\0';


    char buffer[4096];


    while (
        fgets(
            buffer,
            sizeof(buffer),
            pipe
        ) != NULL
    )
    {
        size_t ajout =
            strlen(buffer);


        if (
            longueur + ajout + 128
            >= capacite
        )
        {
            capacite *= 2;


            char *nouveau =
                realloc(
                    resultat,
                    capacite
                );


            if (nouveau == NULL)
            {
                free(resultat);

                pclose(pipe);

                return NULL;
            }


            resultat = nouveau;
        }


        memcpy(
            resultat + longueur,
            buffer,
            ajout
        );


        longueur += ajout;

        resultat[longueur] = '\0';
    }


    int status =
        pclose(pipe);


    int code_retour = -1;


    if (WIFEXITED(status))
    {
        code_retour =
            WEXITSTATUS(status);
    }


    char fin[128];


    snprintf(
        fin,
        sizeof(fin),
        "\n[Commande executee, code retour: %d]",
        code_retour
    );


    size_t taille_fin =
        strlen(fin);


    if (
        longueur + taille_fin + 1
        >= capacite
    )
    {
        capacite =
            longueur + taille_fin + 1;


        char *nouveau =
            realloc(
                resultat,
                capacite
            );


        if (nouveau == NULL)
        {
            free(resultat);

            return NULL;
        }


        resultat = nouveau;
    }


    memcpy(
        resultat + longueur,
        fin,
        taille_fin + 1
    );


    return resultat;
}


// ============================================================
// REPONSE JSON POUR /exec
// ============================================================

void traiter_exec(
    int client_fd,
    const char *request
)
{
    /*
        Trouver le début du body HTTP
    */

    const char *body =
        strstr(
            request,
            "\r\n\r\n"
        );


    if (body == NULL)
    {
        const char *erreur =
            "{\"erreur\":\"Body HTTP manquant\"}";


        envoyer_headers(
            client_fd,
            "400 Bad Request",
            "application/json",
            strlen(erreur)
        );


        envoyer_tout(
            client_fd,
            erreur,
            strlen(erreur)
        );


        return;
    }


    body += 4;


    char commande[2048];


    if (
        !extraire_commande(
            body,
            commande,
            sizeof(commande)
        )
    )
    {
        const char *erreur =
            "{\"erreur\":\"Commande JSON invalide\"}";


        envoyer_headers(
            client_fd,
            "400 Bad Request",
            "application/json",
            strlen(erreur)
        );


        envoyer_tout(
            client_fd,
            erreur,
            strlen(erreur)
        );


        return;
    }


    printf(
        "\nCOMMANDE RECUE : %s\n",
        commande
    );


    char *resultat =
        executer_commande(
            commande
        );


    if (resultat == NULL)
    {
        resultat =
            strdup(
                "Erreur interne"
            );
    }


    printf(
        "RESULTAT :\n%s\n",
        resultat
    );


    char *resultat_json =
        json_escape(
            resultat
        );


    free(resultat);


    if (resultat_json == NULL)
    {
        const char *erreur =
            "{\"erreur\":\"Erreur memoire\"}";


        envoyer_headers(
            client_fd,
            "500 Internal Server Error",
            "application/json",
            strlen(erreur)
        );


        envoyer_tout(
            client_fd,
            erreur,
            strlen(erreur)
        );


        return;
    }


    size_t taille_json =
        strlen(resultat_json) + 32;


    char *json =
        malloc(taille_json);


    if (json == NULL)
    {
        free(resultat_json);
        return;
    }


    snprintf(
        json,
        taille_json,
        "{\"resultat\":\"%s\"}",
        resultat_json
    );


    free(resultat_json);


    envoyer_headers(
        client_fd,
        "200 OK",
        "application/json",
        strlen(json)
    );


    envoyer_tout(
        client_fd,
        json,
        strlen(json)
    );


    free(json);
}


// ============================================================
// MAIN
// ============================================================

int main(void)
{
    // --------------------------------------------------------
    // Création socket
    // --------------------------------------------------------

    int server_fd =
        socket(
            AF_INET,
            SOCK_STREAM,
            0
        );


    if (server_fd < 0)
    {
        perror("socket");

        return EXIT_FAILURE;
    }


    // --------------------------------------------------------
    // Réutilisation du port
    // --------------------------------------------------------

    int opt = 1;


    setsockopt(
        server_fd,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)
    );


    // --------------------------------------------------------
    // Adresse
    // --------------------------------------------------------

    struct sockaddr_in addr;


    memset(
        &addr,
        0,
        sizeof(addr)
    );


    addr.sin_family =
        AF_INET;


    /*
        INADDR_ANY :
        accessible depuis les autres PC du réseau
    */

    addr.sin_addr.s_addr =
        INADDR_ANY;


    addr.sin_port =
        htons(PORT);


    // --------------------------------------------------------
    // Bind
    // --------------------------------------------------------

    if (
        bind(
            server_fd,
            (struct sockaddr *)&addr,
            sizeof(addr)
        ) < 0
    )
    {
        perror("bind");

        close(server_fd);

        return EXIT_FAILURE;
    }


    // --------------------------------------------------------
    // Listen
    // --------------------------------------------------------

    if (
        listen(
            server_fd,
            10
        ) < 0
    )
    {
        perror("listen");

        close(server_fd);

        return EXIT_FAILURE;
    }


    printf(
        "========================================\n"
        " SERVEUR C ACTIF\n"
        " Port : %d\n"
        " Adresse : http://192.168.124.22:%d/\n"
        "========================================\n",
        PORT,
        PORT
    );


    // ========================================================
    // BOUCLE SERVEUR
    // ========================================================

    while (1)
    {
        int client_fd =
            accept(
                server_fd,
                NULL,
                NULL
            );


        if (client_fd < 0)
        {
            perror("accept");

            continue;
        }


        char request[BUFFER_SIZE];


        memset(
            request,
            0,
            sizeof(request)
        );


        ssize_t n =
            read(
                client_fd,
                request,
                sizeof(request) - 1
            );


        if (n <= 0)
        {
            close(client_fd);

            continue;
        }


        request[n] = '\0';


        // ----------------------------------------------------
        // Afficher première ligne HTTP
        // ----------------------------------------------------

        char *fin_ligne =
            strstr(
                request,
                "\r\n"
            );


        if (fin_ligne != NULL)
        {
            *fin_ligne = '\0';

            printf(
                "\nHTTP : %s\n",
                request
            );

            *fin_ligne = '\r';
        }


        // ====================================================
        // OPTIONS
        // ====================================================

        if (
            strncmp(
                request,
                "OPTIONS ",
                8
            ) == 0
        )
        {
            envoyer_headers(
                client_fd,
                "200 OK",
                "text/plain",
                0
            );
        }


        // ====================================================
        // POST /exec
        // ====================================================

        else if (
            strncmp(
                request,
                "POST /exec ",
                11
            ) == 0
        )
        {
            traiter_exec(
                client_fd,
                request
            );
        }


        // ====================================================
        // GET /
        // ====================================================

        else if (
            strncmp(
                request,
                "GET / ",
                6
            ) == 0
        )
        {
            servir_fichier(
                client_fd,
                "index.html",
                "text/html; charset=utf-8"
            );
        }


        // ====================================================
        // GET /index.html
        // ====================================================

        else if (
            strncmp(
                request,
                "GET /index.html ",
                16
            ) == 0
        )
        {
            servir_fichier(
                client_fd,
                "index.html",
                "text/html; charset=utf-8"
            );
        }


        // ====================================================
        // GET /script.js
        // ====================================================

        else if (
            strncmp(
                request,
                "GET /script.js ",
                15
            ) == 0
        )
        {
            servir_fichier(
                client_fd,
                "script.js",
                "application/javascript; charset=utf-8"
            );
        }


        // ====================================================
        // GET /style.css
        // ====================================================

        else if (
            strncmp(
                request,
                "GET /style.css ",
                15
            ) == 0
        )
        {
            servir_fichier(
                client_fd,
                "style.css",
                "text/css; charset=utf-8"
            );
        }


        // ====================================================
        // GET /style2.css
        // ====================================================

        else if (
            strncmp(
                request,
                "GET /style2.css ",
                16
            ) == 0
        )
        {
            servir_fichier(
                client_fd,
                "style2.css",
                "text/css; charset=utf-8"
            );
        }


        // ====================================================
        // FAVICON
        // ====================================================

        else if (
            strncmp(
                request,
                "GET /favicon.ico ",
                17
            ) == 0
        )
        {
            envoyer_headers(
                client_fd,
                "204 No Content",
                "image/x-icon",
                0
            );
        }


        // ====================================================
        // 404
        // ====================================================

        else
        {
            const char *erreur =
                "404 - Ressource introuvable";


            envoyer_headers(
                client_fd,
                "404 Not Found",
                "text/plain; charset=utf-8",
                strlen(erreur)
            );


            envoyer_tout(
                client_fd,
                erreur,
                strlen(erreur)
            );
        }


        close(client_fd);
    }


    close(server_fd);

    return EXIT_SUCCESS;
}
