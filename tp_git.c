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
#define MIN_CLE 100
#define MAX_SUPERFICIE 1000
#define MIN_SUPERFICIE 500
#define NB_WILAYA_TOTAL 58
#define MIN_NB_LIVRETS_ALEATOIRES 3
#define MAX_NB_LIVRETS_ALEATOIRES 6


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
    EcrireDir(fichier, Entete_info(fichier,5),buf);          // ecriture du bloc de queue dans le fichier
    Aff_entete(fichier, 3, Entete_info(fichier,1)+1);        // mise a jour du numero du bloc correspondant a la nouvelle queue dan sl'entete
    buf->suivant = -1;                                  // mise a jour du suivant a nill
    sprintf(buf->tab,"%s","");                          // vider la chaine du buffer
    EcrireDir(fichier, Entete_info(fichier,3), buf);         // ecriture du buffer dans le bloc representatnt la nouvelle queue
    Aff_entete(fichier, 1, Entete_info(fichier,1) + 1);      // incrementation du nombre de bloc alloues
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
    int taille = alea_nb(min_taille,TAILLE_BLOC-min_taille)-2;
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
void alea_enregistrement(enrengistrement* enr, int min , char tab_wilaya[NB_WILAYA_TOTAL][TAILLE_WILAYA+1]){
    char wilaya[TAILLE_WILAYA+1];
    long long int cle;
    char superficie[TAILLE_SUPERFICIE+1];
    char* observation;
    enr->effacer = 0;
    enr->cle = alea_nb(min,MAX_CLE);
    alea_wilaya(tab_wilaya,wilaya);
    strcpy(enr->wilaya,wilaya);
    enr->type = alea_type();
    alea_superficie(superficie);
    strcpy(enr->superficie,superficie);
    alea_observation(&observation);
    strcpy(enr->observation,observation);   
}


// afficher entete
void affichier_entete(lovc* f){
    printf("entete.tete = %d\n",Entete_info(f,1));
    printf("entete.nb_ins = %d\n",Entete_info(f,2));
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
// copier l'entete fu lovc 
void copier_entete(lovc* fich, Entete* entete){
    entete->tete = Entete_info(fich,1);
    entete->nb_ins = Entete_info(fich,2);
    entete->nb_car_sup = Entete_info(fich,3);
    entete->adr_dernier_bloc = Entete_info(fich,4);
    entete->position_dns_dernier_bloc = Entete_info(fich,5);
    entete->nbbloc = Entete_info(fich,6);
}


void main(){ 

    
   
}
