#include <stdio.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#include "client.h"

#define PORT 9600

struct Client Clients[100];
int nbClients = 0;
int decouvertAutorise = -400;

int main(int argc, char *argv[]){
    int sockfd;
    struct sockaddr_in sockaddr_client;
    struct sockaddr_in sockaddr_serv;
    socklen_t addr_client_size = sizeof(struct sockaddr_in);
    char buf[20];
    int realbuf_size;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);

    sockaddr_serv.sin_family = AF_INET;
    sockaddr_serv.sin_addr.s_addr = INADDR_ANY;
    sockaddr_serv.sin_port = PORT;

    bind(sockfd, (struct sockaddr *)&sockaddr_serv, sizeof(struct sockaddr_in));

    listen(sockfd, 5);
    while(1){
        int socket_fils = accept(sockfd, (struct sockaddr *)&sockaddr_client, &addr_client_size);
        if (socket_fils != -1){
            struct Client nClient;
            char *hostname = gethostbyaddr(&sockaddr_client.sin_addr, sizeof(struct in_addr), AF_INET)->h_name;
            int i = 0;
            while (hostname[i] != '\0'){
                nClient.hostname[i] = hostname[i];
                i++;
            }
            printf("Client: %s\n", nClient.hostname);
            bool estClient = false;
            for (int i = 0; i < nbClients; i++){
                if (strcmp(Clients[i].hostname, hostname) == 0){
                    estClient = true;
                }
            }
            int indiceClient = -1;
            if (!estClient){
                char codePin[20];
                char *msgPin = "Merci d'entrer votre code PIN a 4 chiffres: ";
                write(socket_fils, msgPin, strlen(msgPin));
                int size = read(socket_fils, codePin, sizeof(codePin)-1);
                if (size > 0) {
                    codePin[size] = '\0';
                    nClient.codePin = atoi(codePin);
                    printf("Client : %s, Code pin : %d\n", nClient.hostname, nClient.codePin);
                }

                char *msgSolde = "Entrez votre solde initial: ";
                write(socket_fils, msgSolde, strlen(msgSolde));
                char solde[20];
                int sSize = read(socket_fils, solde, sizeof(solde)-1);
                if (sSize > 0){
                    solde[sSize] = '\0';
                    nClient.solde = atof(solde);
                    printf("Client : %s, Solde : %.2f\n", nClient.hostname, nClient.solde);
                }

                indiceClient = nbClients;
                Clients[nbClients] = nClient;
                nbClients++;
                
            }
            if (indiceClient == -1){
                for (int i = 0; i < nbClients; i++){
                    if (strcmp(Clients[i].hostname, hostname) == 0){
                        indiceClient = i;
                        break;
                    }
                }
            }
            char *msgAction = "Montant a retirer/deposer -/+: ";
            write(socket_fils, msgAction, strlen(msgAction));
            char montantAction[20];
            int sMontant = read(socket_fils, montantAction, sizeof(montantAction)-1);
            int nbTentatives = 0;
            bool correctPin = false;
            while (nbTentatives != 3){
                char *msgPin = "Veuillez entrer le code PIN : ";
                write(socket_fils, msgPin, strlen(msgPin));
                char essaiPin[20];
                int n = read(socket_fils, essaiPin, sizeof(essaiPin)-1);
                if (atoi(essaiPin) == Clients[indiceClient].codePin){
                    correctPin = true;
                    break;
                }
                else{
                    nbTentatives++;
                }
            }
            if (!correctPin){
                close(socket_fils);
            }
            if (sMontant > 0){
                float montant = atof(montantAction);
                if (montantAction[0] == '+'){
                    Clients[indiceClient].solde += montant;
                    char depotSucces[100];
                    sprintf(depotSucces, "Montant déposé avec succès. Nouveau solde: %.2f\n", Clients[indiceClient].solde);
                    write(socket_fils, depotSucces, strlen(depotSucces));
                }
                else{
                    if (Clients[indiceClient].solde + montant < decouvertAutorise){
                        char *msgFondsInsuffisants = "Fonds insuffisants !\n";
                        write(socket_fils, msgFondsInsuffisants, strlen(msgFondsInsuffisants));
                    } else{
                        Clients[indiceClient].solde += montant;
                        char retraitSucces[100];
                        sprintf(retraitSucces, "Retrait effectué avec succès. Nouveau solde %.2f\n", Clients[indiceClient].solde);
                        write(socket_fils, retraitSucces, strlen(retraitSucces));
                    }
                }
                printf("Client : %s, Opération : %f, Nouveau solde : %f\n", 
                            Clients[indiceClient].hostname, montant, Clients[indiceClient].solde);
            } 
            close(socket_fils);
        }
        else{
            perror("Accept");
        }
    }
}