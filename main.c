//
//  main.c
//  messagerie
//
//  Created by Quentin Beauchene on 10/9/18.
//  Copyright © 2018 Quentin Beauchene. All rights reserved.
//

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

#define NUMERO_PORT_SERVEUR 30000

int main(int argc, char * argv[]) {
    
    /* couple adresse IP, numero de port serveur */
    struct sockaddr_in coupleAdresseIPNumeroPortServeur;
    /* socket client */
    int socketClient;
    /* longueur de la reponse recue */
    int longueurReponse;
    /* longueur du couple adresse IP, numero de port du serveur */
    socklen_t longueurServeur;
    /* adresse IP du Serveur */
    long int adresseIPServeur;
    char lettre, lettreReponse;
    
    /* Tester le nombre d'arguments */
    if (argc != 2)
    {fprintf(stderr,"usage : clientMinimumTcp <adresse_IP_serveur>\nexemple : clientMinimumTcp 192.168.10.1\n"); return -1;}
    
    /* Initialiser les structures a des octets de valeurs */
    memset(&coupleAdresseIPNumeroPortServeur,0,sizeof(struct sockaddr_in));
    
    /* Initialiser l'adresse IP et le numero de port du serveur */
    
    /* adresse IP au format reseau */
    if (inet_pton(AF_INET, argv[1], (void *)&adresseIPServeur) < 0)
    {perror("inet_pton"); return -1;}
    
    /* socket du domaine reseau */
    coupleAdresseIPNumeroPortServeur.sin_family = AF_INET;
    
    /* adresse IP */
    coupleAdresseIPNumeroPortServeur.sin_addr.s_addr = adresseIPServeur;
    
    /* affecter le numero de port de l'application */
    coupleAdresseIPNumeroPortServeur.sin_port = htons(NUMERO_PORT_SERVEUR);
    
    /* Creer la socket client en mode TCP */
    if ((socketClient = socket(PF_INET,SOCK_STREAM,0)) < 0)
    {perror("socket");return -1;}
    
    /* Effectuer une demande de connexion */
    longueurServeur = sizeof(struct sockaddr_in);
    if (connect(socketClient, (struct sockaddr *)&coupleAdresseIPNumeroPortServeur,
                longueurServeur) < 0)
    {perror("connect");return -1;}
    
    /* demander à l'utilisateur de formuler une requete */
    printf("Entrez une lettre :");
    scanf("%c",&lettre);
    
    /* Envoyer la requete */
    if (send(socketClient, &lettre, sizeof(lettre), 0) == -1)
    {perror("sendto");return -1;}
    
    /* Recevoir la reponse */
    if (recv(socketClient,&lettreReponse, sizeof(unsigned char),0) == -1)
    {perror("recvfrom");return -1;}
    
    /* Afficher la reponse */
    printf("J'ai émis : %c, j'ai reçu : %c\n",lettre, lettreReponse);
    
    /* Fermer la connexion */
    if (close(socketClient) < 0)
    {perror("close");return -1;}
    
    return 0;
}
