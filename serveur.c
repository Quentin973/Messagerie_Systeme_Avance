//
//  serveur.c
//  messagerie
//
//  Created by Quentin Beauchene on 10/9/18.
//  Copyright © 2018 Quentin Beauchene. All rights reserved.
//

#include "serveur.h"
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

/* definition des constantes symboliques */
/* taille de la file d'attente de demandes de connexion */
#define TAILLE_FILE_DATTENTE_CONNEXION 5
#define NUMERO_PORT_SERVEUR 30000

/* routine d'interception du signal SIGCHLD emis par les fils */
void intercepterSignalSIGCHLD(int numeroSignalRecu);

/* prototypes des fonctions */
int communiquerAvecLeClient(int socketConnectee);

int main() {
    
    /* couple adresse IP, numero de port serveur */
    struct sockaddr_in coupleAdresseIPNumeroPortServeur;
    
    /* couple adresse IP, numero de port client */
    struct sockaddr_in coupleAdresseIPNumeroPortClient;
    
    /* socket permettant d'effectuer une demande de connexion */
    int socketDemandeDeConnexion;
    
    /* socket dediee a une connexion etablie */
    int socketConnectee;
    
    /* longueur du couple adresse IP, numero de port du client */
    socklen_t longueurClient;
    int optval
    ;
    /* Mise en place de l'interception de SIGCHLD emis par les fils */
    signal(SIGCHLD,intercepterSignalSIGCHLD);
    
    /* Initialiser les structures a des octets de valeurs */
    memset(&coupleAdresseIPNumeroPortServeur,0,sizeof(struct sockaddr_in));
    memset(&coupleAdresseIPNumeroPortClient,0,sizeof(struct sockaddr_in));
    
    /* Creer la socket serveur en mode TCP */
    if ((socketDemandeDeConnexion = socket(PF_INET,SOCK_STREAM,0)) < 0)
    {perror("socket");return -1;}
    
    // Reutiliser le meme port en cas d'interruption brutal du serveur
    optval = 1;
    setsockopt(socketDemandeDeConnexion, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    
    /* Associer la socket serveur a un couple adresse IP, numero de port */
    /* socket du domaine reseau */
    coupleAdresseIPNumeroPortServeur.sin_family = AF_INET;
    
    /* INADDR_ANY pour accepter les requetes de toutes les interfaces reseau du serveur */
    coupleAdresseIPNumeroPortServeur.sin_addr.s_addr = htonl(INADDR_ANY);
    
    /* affecter le numero de pott de l'application */
    coupleAdresseIPNumeroPortServeur.sin_port = htons(NUMERO_PORT_SERVEUR);
    if (bind(socketDemandeDeConnexion,
             (struct sockaddr *)(&coupleAdresseIPNumeroPortServeur),
             sizeof(struct sockaddr_in)) < 0)
    {perror("bind");return -1;}
    
    /* Creer une file d'attente de demandes de connexion */
    if (listen(socketDemandeDeConnexion,TAILLE_FILE_DATTENTE_CONNEXION) <0)
    {perror("listen");return -1;}
    longueurClient = sizeof(struct sockaddr_in);
    
    while(1) {
        
        /* Attendre une demande de connexion du client */
        socketConnectee = accept(socketDemandeDeConnexion,
                                 (struct sockaddr *)(&coupleAdresseIPNumeroPortClient),
                                 &longueurClient);
        if (socketConnectee < 0) {
            perror ("accept");
        } else {
            /* Creer un fils pour traiter la communication */
            if (fork() == 0) {
                /* Dans le fils */
                /* Fermer dans le fils la socket serveur permettant d'effectuer la demande */
                if (close(socketDemandeDeConnexion)<0)
                {perror("close");return -1;}
                
                /* Communiquer avec le client */
                if (communiquerAvecLeClient(socketConnectee) < 0 )
                {fprintf(stderr,"communiquerAverLeClient : echec\n"); close(socketConnectee); return
                    -1;}
                
                /* Fermer dans le fils la socket connectee */
                if (close(socketConnectee) < 0)
                {perror("close");return -1;}
                return 0;
            }
            /* dans le pere */
            /* fermer dans le pere la socket connectee */
            if(close(socketConnectee) < 0)
            {perror("close");return -1;}
        }
    }
    return 0;
}

void intercepterSignalSIGCHLD(int numeroSignalRecu) {
    wait(NULL);
}

int communiquerAvecLeClient(int socketConnectee) {
    char lettre;
    char lettreReponse;
    /* Recevoir la requête */
    if (recv(socketConnectee, &lettre, sizeof(lettre), 0) == -1)
    {perror("recvfrom");return -1;}
    
    /* Traiter la requete */
    lettreReponse = lettre + 1;
    
    /* Envoyer la réponse */
    if (send(socketConnectee, &lettreReponse, sizeof(lettreReponse), 0) == -1)
    {perror("sendto");return -1;}
    return 0;
}
