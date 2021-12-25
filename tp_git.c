#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define TAILLE_BLOC 150        
#define TAILLE_WILAYA 14
#define TAILLE_SUPERFICIE 10
#define MAX_CLE 1000
#define TAILLE_TYPE 1
#define TAILLE_CLE 10
#define MAX_SUPERFICIE 1000
#define MIN_SUPERFICIE 500
#define NB_WILAYA_TOTAL 58
#define MIN_NB_LIVRETS_ALEATOIRES 3
#define MAX_NB_LIVRETS_ALEATOIRES 6
#define MIN_LIVRETS_ALEATOIRES 2
#define MAX_LIVRETS_ALEATOIRES 5

//_______________________________________//
//_______________ Structures ______________//
//_______________________________________//

//Declaration de l'enrengistrement 
typedef struct enrengistrement
{
    long long int cle;                    // la clee unique 
    int effacer;                          // champ de supression logiqe
    char wilaya[TAILLE_WILAYA+1];           // champ wilaya ou se trouve le bien
    char superficie[TAILLE_SUPERFICIE+1];   // la superficie du bien
    char type;               // peut etre :terrain(T),maison(M),appartement(A),immeuble(I)
    char* observation;                    // pour la caracterisation du bien 
}enrengistrement;

//Declaration du bloc do LOVC
typedef struct tbloc
{
    char tab[TAILLE_BLOC+1]; 
    int suivant;                // numero du bloc suivant
}maillon;

//Declaration de l'entete de la LOVC
typedef struct Entete
{
    int tete;
    int nb_ins;                      // nombre de caracteres iserees
    int nb_car_sup;                  // nombre de caracteres suprimees 
    int adr_dernier_bloc;            // numero du dernier bloc 
    int position_dns_dernier_bloc;   // position de dernier caractere inseree dans le dernier bloc du fichier
    int nbbloc;                      // nombre de blocs dans le ficheir 
}Entete;

//Declaration de la structure LOVC
typedef struct lovc 
{
    FILE* fichier;                  // pointeur vers le fichier
    Entete entete;                  // l'entete du fichier lovc
}lovc;


//_______________________________________//
//__________ Machine abstrete ___________//
//_______________________________________//

// Lecture du bloc numero i du fichier f dans la variable buf               
void LireDir(lovc *f, int i, maillon* buf){
    fseek(f->fichier, sizeof(Entete)+ sizeof(maillon)*(i-1), SEEK_SET);  // positionnement dans le premier caractere du bloc i 
    fread(buf, sizeof(maillon), 1, f->fichier);             //  lire tous le bloc 
    rewind(f->fichier);                                     //  reposionnement dans le debut du fichier
}

// Ecriture du contenue de buf dans l'placement i du fichier
void EcrireDir(lovc* f, int i, maillon *buf){
    fseek(f->fichier, sizeof(Entete)+ sizeof(maillon)*(i-1), SEEK_SET);  // positionnement dans le premier caractere du bloc i 
    fwrite(buf, sizeof(maillon), 1, f->fichier);                          // ecrire dans le bloc i
}

// Affecter la valeur "val" dans le champ "i"                   ***** marche *****
void Aff_entete(lovc* f, int i, int val){
    switch (i)
    {
    case 1:
        f->entete.tete = val;
        break;
    case 2:
        f->entete.nb_ins = val;
        break;
    case 3:
        f->entete.nb_car_sup = val;
        break;
    case 4:
        f->entete.adr_dernier_bloc = val;
        break;
    case 5:
        f->entete.position_dns_dernier_bloc = val;
        break;
    case 6:
        f->entete.nbbloc = val;
        break;
    default:
        break;
    }
}

// retourne la valeur "val" de l'entete                             ***** marche *****
int Entete_info(lovc* f, int i){
    switch (i)
    {
    case 1:
        return f->entete.tete;
        break;
    case 2:
        return f->entete.nb_ins;
        break;
    case 3:
        return f->entete.nb_car_sup;
        break;
    case 4:
        return f->entete.adr_dernier_bloc;
        break;
    case 5:
        return f->entete.position_dns_dernier_bloc;
        break;
    case 6:
        return f->entete.nbbloc;
        break;
    default:
        break;
    }

}
// ouvrire un fichier lovc 
lovc* Ouvrir(lovc* f, char nom[], char mode){
    maillon buf;
    lovc *fichier = malloc(sizeof(lovc));                                                                    
    if (mode == 'A' || mode == 'a')
    { 
        fichier->fichier = fopen(nom,"rb+");                                 // ouverture du fichier en mode binaire lecture et ecriture
        Entete entete;
        fread(&entete,sizeof(Entete),1,fichier->fichier);    // chargement de l'entete enregistree en debut de fichier
        Aff_entete(fichier, 1, entete.tete);                      // mettre tete au premier bloc
        Aff_entete(fichier, 2, entete.nb_ins);                    // mise a zero le nombre de caractere inserer puisque le bloc est vide
        Aff_entete(fichier, 3, entete.nb_car_sup);                // aucun caractere n'a encore ete supprime
        Aff_entete(fichier, 4, entete.adr_dernier_bloc);          // la queue est la tete puisque le fichier est vide
        Aff_entete(fichier, 5, entete.position_dns_dernier_bloc); // le premier caractere du ficheir correspond a la position libre puisqu'il est nouveau
        Aff_entete(fichier, 6, entete.nbbloc);
    }
    else{
        if ((mode == 'N') || (mode == 'n'))
        {
            
            fichier->fichier = fopen(nom,"wb+");                               // ouverture du fichier en mode binaire  ecriture
            Aff_entete(fichier, 1, 1);                                        // mettre tete au premier bloc
            Aff_entete(fichier, 2, 0);                                        // mise a zero le nombre de caractere inserer puisque le bloc est vide
            Aff_entete(fichier, 3, 0);                                        // aucun caractere n'a encore ete supprime
            Aff_entete(fichier, 4, 1);                                        // la queue est la tete puisque le fichier est vide
            Aff_entete(fichier, 5, 0);                                        // le premier caractere du ficheir correspond a la position libre puisqu'il est nouveau
            Aff_entete(fichier, 6, 1);
            fwrite(&(fichier->entete), sizeof(Entete), 1, fichier->fichier);      // enregistrement de l'entete dans le fichier
            buf.suivant = -1;                                               // le suivant du premier bloc a NULL
            sprintf(buf.tab,"%s","");                                       // initialisation du buffer a chaine vide
            EcrireDir(fichier, 1, &buf);                                      // ecriture du premier bloc dans le fichier     
            }
        else                                                // format d'ouverture incorrecte
        {
            printf("format d'ouverture impossible");
        }
        
    }
    
    return(fichier);                                       // renvoyer la structure cree
}

// procedure de fermeture du fichier
void fermer(lovc *fich)  
{
    rewind(fich->fichier);                                  // repositionnement du curseur en debut de fichier
    fwrite(&(fich->entete),sizeof(Entete),1,fich->fichier);    // sauvegarde de la derniere version de l'entete de la strucuture L7OVC
                                                            // dans le fichier
    rewind(fich->fichier);                                  // repositionnement du curseur en debut de fichier
    fclose(fich->fichier);                                  // fermeture du fichier
}
void  alloc_bloc(lovc *fichier) 
{   
    maillon *buf = malloc(sizeof(maillon));             // allocation du Buffer
    LireDir(fichier, Entete_info(fichier,4), buf);           // lecture du bloc correspondant a la queue
    buf->suivant = Entete_info(fichier,6) + 1;               // mise a jour de suivant de la queue au bloc correspondant a la nouvelle queue
    EcrireDir(fichier, Entete_info(fichier,4),buf);          // ecriture du bloc de queue dans le fichier
    Aff_entete(fichier, 4, Entete_info(fichier,6)+1);        // mise a jour du numero du bloc correspondant a la nouvelle queue dan sl'entete
    buf->suivant = -1;                                  // mise a jour du suivant a nill
    sprintf(buf->tab,"%s","");                          // vider la chaine du buffer
    EcrireDir(fichier, Entete_info(fichier,4), buf);         // ecriture du buffer dans le bloc representatnt la nouvelle queue
    Aff_entete(fichier, 6, Entete_info(fichier,6) + 1);      // incrementation du nombre de bloc alloues
    Aff_entete(fichier,5,0);                                // la position dans le dernier bloc est 0
}

//_______________________________________//
//______________ Fonctions ______________//
//_______________________________________//

// retourne la longueure d'un nembre                            ***** marche *****
int longueur(long long int n){
    int count = 0;
    if(n==0){
        return 1;
    }
    while (n != 0)
    {   
        n /= 10;
        count ++;
    }
    return count;
}

// convertie un entier a une chaine de caracteres               ***** marche *****
void int_to_char(long long int number, char* sortie){

    int save = longueur(number);
    for (int i = longueur(number) - 1; i >= 0; i--)
    {
        sortie[i] = (number%10) +'0';
        number /= 10;
    }
    sortie[save] = '\0';
}

// convertie un enregistrement a une chaine de caracteres               ***** marche *****
void enr_to_string(enrengistrement info, char** string){

    *string = malloc(sizeof(enrengistrement));
    char cle[TAILLE_CLE+1];
    int_to_char(info.cle, cle);
    strcpy(*string, cle);
    strcat(*string, "$");           
    strcat(*string, info.wilaya);
    strcat(*string, "$");
    char type[2];
    type[0] = info.type;
    type[1] = '\0';
    strcat(*string, type);
    strcat(*string, "$");
    strcat(*string, info.superficie);
    strcat(*string, "$");
    strcat(*string, info.observation);
    strcat(*string, "@");
}

// a+(rand())%(b-a) 
/***     $ : entre deux champs     @  : entre deux livrets
 *  les fonctions a programmer sont :
 *          . int alea_nb(int a , int b) // retourne un nembre aleatoire entre a et b                    ****
 *          . char* alea_wilaya() // retourne le nom d'une wilaya aleatoirement
 *          . char* alea_superficie() // retourne une chaine pour le champs superficie


*/

 //retourne un nembre aleatoire entre a et b                        ***** marche *****
 long long int alea_nb(long long int a ,long long int b){    //   srand(time(NULL));  au debut de la fonction appelante 
     return a + rand()%(b+1-a);
}
// retourne une chaine pour le champs superficie                     ***** marche ***** 
void alea_superficie(char* superficie){
    char s[TAILLE_SUPERFICIE+1];
    int_to_char(alea_nb(MIN_SUPERFICIE,MAX_SUPERFICIE),s);
    strcpy(superficie,s); 
}

// retourne un type aleatoire                                           ***** marche *****
char alea_type(){
    int i = 1 + rand()%4;
    
    switch (i)
    {
    case 1:
        return'T';
        break;
    case 2:
        return'M';
        break;
    case 3:
        return'A';
        break;
    case 4:
        return'I';
        break;
    
    default:
        printf("un probleme au niveau de la fonction alea_type \n ");
        break;
    }
}
//generer une observation aleatoirement                                      ***** marche *****     
void alea_observation(char** obs){
    int min_taille = TAILLE_CLE+TAILLE_SUPERFICIE+TAILLE_TYPE+TAILLE_WILAYA;
    int taille = alea_nb(min_taille,TAILLE_BLOC-min_taille-2);
    printf("taille  = %d\n",taille); 
    *obs = malloc(sizeof(char)*taille);
    int i,i1 , i2, i3 , count = 0;
    char c[2];
    c[0]=alea_nb(65,90);
    c[1]='\0';
    strcpy(*obs,c);
    for(count = 1 ; count<taille ; count++)
    {
        i = alea_nb(0,3);
        i1 = alea_nb(48,57);
        i2 = alea_nb(65,90);
        i3 = alea_nb(97,122);
        switch (i)
        {
        case 0:
            c[0]= 32 ;
            strcat(*obs,c); 
            break;
        case 1:
            c[0]= i1 ;
            strcat(*obs,c);
            break;
        case 2:
            c[0]= i2 ;
            strcat(*obs,c);
            break;
        case 3:
            c[0]= i3 ;
            strcat(*obs,c);
            break;    
        default:
            printf("un probleme dans la fonction alea_observation \n");
            break;
        }

    }
    *(*obs + count)='\0';

}
/////////////////////

// generer le tableau des wilaya                                          ***** marche *****
void generer_tab_wilaya(char tab[NB_WILAYA_TOTAL][TAILLE_WILAYA+1] ){
   
    FILE* f = fopen("wilaya.txt","r");
    if (f==NULL){
        printf("le fichier n'a pas etait ouvert \n");
        return;
    }
    else
    {
        char w[TAILLE_WILAYA+1];
        for(int i = 0 ; i < NB_WILAYA_TOTAL ; i++){
            fscanf(f,"%s",w);
             for (int j = 0 ; j<=strlen(w)+1; j++) 
             { 
                 tab[i][j] = w[j];
             } 
        }
    }   
}

// generer une wilaya aleatoirement                             ***** marche ***** 
void alea_wilaya(char tab_wilaya[NB_WILAYA_TOTAL][TAILLE_WILAYA+1], char* wilaya){
    int i =alea_nb(0,NB_WILAYA_TOTAL-1);
    strcpy(wilaya,tab_wilaya[i]);
}

// generer un enregistrement aleatoire                          ***** marche *****
void alea_enregistrement(enrengistrement* enr, int* min , char tab_wilaya[NB_WILAYA_TOTAL][TAILLE_WILAYA+1]){
    char wilaya[TAILLE_WILAYA+1];
    long long int cle;
    char superficie[TAILLE_SUPERFICIE+1];
    char* observation;
    enr->effacer = 0;
    enr->cle = alea_nb(*min,MAX_CLE);
    *min = enr->cle+1;
    alea_wilaya(tab_wilaya,wilaya);
    strcpy(enr->wilaya,wilaya);
    enr->type = alea_type();
    alea_superficie(superficie);
    strcpy(enr->superficie,superficie);
    alea_observation(&observation);
    enr->observation = malloc(sizeof(char)*(strlen(observation)+1));
    strcpy(enr->observation,observation);  
}

// copier l'entete fu lovc 
void copier_entete(lovc* fich, Entete* entete){
    entete->tete = Entete_info(fich,1);
    entete->nb_ins = Entete_info(fich,2);
    entete->nb_car_sup = Entete_info(fich,3);
    entete->adr_dernier_bloc = Entete_info(fich,4);
    entete->position_dns_dernier_bloc = Entete_info(fich,5);
    entete->nbbloc = Entete_info(fich,6);
}

//ecrit un enregistrement dans un bloc 
void ecrire_enr_dans_bloc(lovc*f , Entete* head , enrengistrement* enr , maillon* buf){

    char* copy;
    int dernier = head->adr_dernier_bloc;
    int position = head->position_dns_dernier_bloc;
    int i = 0 ;
    enr_to_string(*enr,&copy);
    if(strlen(copy)<(TAILLE_BLOC-head->position_dns_dernier_bloc)){
            if(position == 0){
                strcpy(buf->tab,copy);
            }           
            else{
                strcat(buf->tab,copy);
            }
            head->position_dns_dernier_bloc += strlen(copy);
            head->nb_ins += strlen(copy);      

        }
        else{
            if(position != TAILLE_BLOC){
            for(i = 0 ; i < TAILLE_BLOC-head->position_dns_dernier_bloc ; i++){
                buf->tab[position] = copy[i];
                position++;
            }
            }
            buf->tab[TAILLE_BLOC]='\0';
            buf->suivant = -1;
            EcrireDir(f,head->adr_dernier_bloc,buf);
            alloc_bloc(f);
            copier_entete(f,head); 
            sprintf(buf->tab,"%s","");
            buf->suivant = -1;
            position = 0; 
            while(i<strlen(copy)){
                buf->tab[position] = copy[i];
                position++;
                i++;
            }
            head->position_dns_dernier_bloc = position;
            head->nb_ins += strlen(copy);           
            
        }    

}
// creer le fichier lovc  « Livrets_National.bin »
lovc* creer_lovc(void){
    lovc* f = malloc(sizeof(lovc));
    f = Ouvrir(f,"Livrets_National.bin",'N');
    Entete head;
    copier_entete(f,&head);
    maillon buf;
    enrengistrement enr;
    char tab_wilaya[NB_WILAYA_TOTAL][TAILLE_WILAYA+1];
    generer_tab_wilaya(tab_wilaya);
    int nb_livrets = alea_nb(MIN_LIVRETS_ALEATOIRES,MAX_LIVRETS_ALEATOIRES);
    int min_cle = 1;    //la clee minimale
    for(int i = 0;i<nb_livrets;i++){
        alea_enregistrement(&enr,&min_cle,tab_wilaya);
        ecrire_enr_dans_bloc(f,&head,&enr,&buf);
    }
    EcrireDir(f,head.adr_dernier_bloc,&buf);
    rewind(f->fichier);
    fwrite(&head,sizeof(Entete),1,f->fichier);
    fermer(f);
    return f ; 

}


// afficher entete
void affichier_entete(lovc* f){
    printf("entete.tete = %d\n",Entete_info(f,1));
    printf("entete.nb_ind = %d\n",Entete_info(f,2));
    printf("entete.nb_sup = %d\n",Entete_info(f,3));
    printf("entete.adr_dernier_bloc = %d\n",Entete_info(f,4));
    printf("entete.adr_dns_dernier_bloc = %d\n",Entete_info(f,5));
    printf("entete.nbbloc = %d\n",Entete_info(f,6));
}
// aficher le ième bloc
void afficher_bloc(lovc* fich , int i ){
    maillon buf;
    LireDir(fich,i,&buf);
    if(strlen(buf.tab)==0)
        printf("le buffers est vide \n");
    else
        printf("%s \n",buf.tab);
}

/////////////////////////////////////////////////::
/////////////////////////////////////////////////::

// question 3 

// extraire l'enregistrement à partire de la position p dans le bloc k dans lovc
void extraire_enr_string(lovc* f ,int* k , int* p , char** enr_string){
    maillon buf;
    int i = 0;
    char copy[TAILLE_BLOC];
    copy[0]='\0';
    *enr_string = malloc(sizeof(enrengistrement));
    *enr_string[0]='\0';
    if(Entete_info(f,1)=1 && Entete_info(f,2)!=0){
        if(*k==Entete_info(f,1) && p==0){
            LireDir(f,*k,&buf);
            strncpy(*enr_string , buf.tab ,strlen(buf.tab)-strlen(strchr(buf.tab,'@')));// copier toute la chaine avant trouver 
                                                                                        //un caractre de fin d'enregistrement '@' 
            *p = strlen(*enr_string);                                                                                                                                                          
        }
        else{
            LireDir(f,*k,&buf);
            if(buf.tab[*p]!='@'){
                if(strchr(buf.tab,'@') == NULL){
                    if(buf.suivant==-1){
                        *k = -1;
                        return;
                    }
                    else{
                        *k = buf.suivant;
                        LireDir(f,buf.suivant,&buf);
                        strcpy(copy,strchr(buf.tab,'@'));
                        *p = strlen(copy)-1; // positionner p sur le caractere '@'
                        
                    }
                }
            }
            if(buf.tab[*p+1]=='\0'){
                *k = -1;
                return;            
            }
            if(strchr(buf.tab,'@') == NULL){
                int ref = *p;
                for(i = 0 ; i < TAILLE_BLOC-ref ; i++){
                    *enr_string[i]=buf.tab[*p];
                    *p++;
                }
                *enr_string[i] = '\0';
                *k = buf.suivant;
                LireDir(f,buf.suivant,&buf);
                strncat(*enr_string , buf.tab , strlen(buf.tab)-strlen(strchr(buf.tab,'@')));   // la chaine avant trouver à nouveau un '@'
                *p = strlen(*enr_string)-i; // positionner p sur le caractere '@'           
            }
            else{
                strncpy(*enr_string , buf.tab , strlen(buf.tab)-strlen(strchr(buf.tab,'@')));  // la chaine avant trouver à nouveau un '@'
                *p += strlen(*enr_string) + 1;          // positionner p sur le prochain '@'
            }
        }
    }
    else{
        *k = -1;
        *p = 0;
    }
}
// chaine to enr                                ***** marche ***** 
void chaine_to_enr(char* enr_string , enrengistrement* enr){
    int i = 1;
    char* copy = malloc(sizeof(char)*strlen(enr_string));
    strcpy(copy , enr_string);
    char* champs =  malloc(sizeof(char)*strlen(enr_string));
    while(i<=6){
        if(i != 6){
        strncpy(champs , copy , strlen(copy)-strlen(strchr(copy,'$')));
        strcpy(copy,strchr(copy , '$'));
        strcpy(copy,strchr(copy , copy[1]));
        }
        switch (i)
        {
        case 1:
            enr->cle = atoi(champs);            
            break;
        case 2:
            enr->effacer = atoi(champs);
            break;
        case 3:
            strcpy(enr->wilaya,champs);
            break;
        case 4:
            strcpy(enr->superficie , champs);
            break;
        case 5:
            enr->type = champs[0];
            break;
        case 6:
            (enr->observation)= malloc(sizeof(char)*(strlen(copy)+1));
            strcpy(enr->observation,copy);
            break;    

        default:
            printf("probleme dans la fonction chaine_to_enr\n ");
            break;
        }
        i++;
    }
}

//rechercher un livret
void rechercher_livrets(int cle , lovc* f , int* trouve , int* num_bloc , int* position ,int* effacer){
    enrengistrement info;
    maillon buf;
    *trouve = 0;
    *num_bloc = 1;
    *position = 0;
    *effacer = 1;
    int stop = 0;
    int save_position = 0;
    int save_bloc = 1;
    char* copy;
    if(Entete_info(f,1)!=0 && Entete_info(f,2)!=0){           // le fichier existe 
        do
        {   save_position = *position;
            save_bloc = *num_bloc;
            extraire_enr_string(f,*num_bloc,*position,&copy);
            chaine_to_enr(copy,&info);
            if(info.cle <= cle){
                stop = 1;       // arreter la recherche
                *position = save_position+1;
                *num_bloc = save_bloc+1;
                if(info.cle == cle ){
                    *effacer = info.effacer;
                    *trouve = 1;
                }
                else{
                    *effacer = 1;
                    *trouve = 0;                    
                }
            }
            if (*num_bloc == -1)
                stop = 1;            
        } while (!stop && !trouve );
        if(stop && *num_bloc==-1){
            if(Entete_info(f,5)==TAILLE_BLOC){
                *num_bloc = Entete_info(f,4)+1;
                *position = 0;
            }
            else{
                *num_bloc = Entete_info(f,4);
                *position = Entete_info(f,5);
            }
        }   
    }
}



void main(){ 

    // tester alea_observation
    /*srand(time(NULL));                 
   char* obseration;
   alea_observation(&obseration);
   printf("%s \n",obseration);*/
    

    //tester alea_wilaya
    /*srand(time(NULL));                
    char tab_wilaya[NB_WILAYA_TOTAL][TAILLE_WILAYA+1];
    generer_tab_wilaya(tab_wilaya);
    char wilaya[TAILLE_WILAYA+1];
    alea_wilaya(tab_wilaya,wilaya);
    printf("%s \n", wilaya);*/


   
    
   
}

