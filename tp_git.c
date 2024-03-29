#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define TAILLE_BLOC 150        
#define TAILLE_WILAYA 14
#define TAILLE_SUPERFICIE 10
#define MAX_CLE 9999999999
#define MIN_CLE 100
#define TAILLE_TYPE 1
#define TAILLE_CLE 10
#define MAX_SUPERFICIE 9999999999
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
    char taille_enr[4];                     // la taille d'un enregistrement est sur 3 caracteres
    char cle[TAILLE_CLE+1];                 // la clee unique 
    int effacer;                            // champ de supression logiqe
    char wilaya[TAILLE_WILAYA+1];           // champ wilaya ou se trouve le bien
    char superficie[TAILLE_SUPERFICIE+1];   // la superficie du bien
    char type;                              // peut etre :terrain(T),maison(M),appartement(A),immeuble(I)
    char* observation;                      // pour la caracterisation du bien 
}enrengistrement;

//Declaration du bloc do LOVC
typedef struct tbloc
{
    char tab[TAILLE_BLOC+1]; 
    int suivant;                            // numero du bloc suivant
}maillon;

//Declaration de l'entete de la LOVC
typedef struct Entete
{
    int tete;
    int nb_ins;                             // nombre de caracteres iserees
    int nb_car_sup;                         // nombre de caracteres suprimees 
    int adr_dernier_bloc;                   // numero du dernier bloc 
    int position_dns_dernier_bloc;          // position de dernier caractere inseree dans le dernier bloc du fichier
    int nbbloc;                             // nombre de blocs dans le ficheir 
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

// Affecter la valeur "val" dans le champ "i"                  
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

// Retourne la valeur "val" de l'entete                             
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

// Retourne la longueure d'un nembre                            
int longueur(long long int n){
    int count = 0;
    if(n == 0){
        return 1;
    }
    while (n != 0)
    {   
        n /= 10;
        count ++;
    }
    return count;
}

//Convertie un entier a une chaine de caracteres               
void int_to_char(long long int number, char* sortie){

    int save = longueur(number);
    for (int i = longueur(number) - 1; i >= 0; i--)
    {
        sortie[i] = (number%10) +'0';
        number /= 10;
    }
    sortie[save] = '\0';
}
/* void char_to_int(char *entree, long long int){

} */


// convertie un enregistrement a une chaine de caracteres               
void enr_to_string(enrengistrement info, char sortie[TAILLE_BLOC]){

    char string [TAILLE_BLOC];
    //*sortie = malloc(sizeof(enrengistrement));
    strcpy(string,info.taille_enr);
    strcat(string,"$");    
    char cle[TAILLE_CLE+1];
    strcat(string, info.cle);
    strcat(string, "$");  
    if(info.effacer == 0){
        strcat(string,"0$");
    }     
    else{
        strcat(string,"1$");
    }    
    strcat(string, info.wilaya);
    strcat(string, "$");
    char type[2];
    type[0] = info.type;
    type[1] = '\0';
    strcat(string, type);
    strcat(string, "$");
    strcat(string, info.superficie);
    strcat(string, "$");
    strcat(string, info.observation);
    strcat(string, "@");
    string[strlen(string)] = '\0';
    strcpy(sortie, string);
    
}
//__________________________________________________//
//______________ Fonctions aleatoires ______________//
//__________________________________________________//

//retourne un nembre aleatoire entre a et b                        
long long int alea_nb(long long int a ,long long int b){    //   srand(time(NULL));  au debut de la fonction appelante 
    return a + rand()%(b+1-a);
}
// Retourne une chaine pour le champs superficie                     
void alea_superficie(char* superficie){
    char s[TAILLE_SUPERFICIE+1];
    int_to_char(alea_nb(MIN_SUPERFICIE,MAX_SUPERFICIE),s);
    strcpy(superficie, s);
    char c[TAILLE_SUPERFICIE+1];
    for (int i = 0; i < TAILLE_SUPERFICIE - strlen(superficie); i++)
    {
        c[i] = '0';
    }
    c[TAILLE_SUPERFICIE - strlen(superficie)] = '\0';
    strcat(c, superficie);
    strcpy(superficie, c);
}

// Retourne un type aleatoire                                           
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
//generer une observation aleatoirement                                        
void alea_observation(char** obs,int* longueur){
    int min_taille = TAILLE_CLE+TAILLE_SUPERFICIE+TAILLE_TYPE+TAILLE_WILAYA;
    *longueur = alea_nb(min_taille,TAILLE_BLOC-min_taille-2);
    int taille = *longueur;
    //printf("taille  = %d\n",taille); 
    *obs = malloc(sizeof(char)*(taille+1));
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

// generer le tableau des wilaya                                          
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

// generer une wilaya aleatoirement                             
void alea_wilaya(char tab_wilaya[NB_WILAYA_TOTAL][TAILLE_WILAYA+1], char wilaya[TAILLE_WILAYA+1]){
    int i = alea_nb(0, NB_WILAYA_TOTAL);
    strcpy(wilaya,tab_wilaya[i]);
    for (int i = strlen(wilaya); i < TAILLE_WILAYA; i++)
    {
        wilaya[i] = '.'; 
    }
    wilaya[TAILLE_WILAYA] = '\0';  
}
void alea_cle(int* min, char c[TAILLE_CLE+1]){
    int x = alea_nb(*min, MAX_CLE);
    *min = x;
    char cle[TAILLE_CLE+1];
    for (int i = 0; i < TAILLE_CLE - longueur(x); i++)
    {
        cle[i] = '0';
    }
    cle[TAILLE_CLE - longueur(x)] = '\0';
    int_to_char(x, c);
    strcat(cle, c);
    strcpy(c, cle);
}

// generer un enregistrement aleatoire                          
void alea_enregistrement(enrengistrement* enr, int* min , char tab_wilaya[NB_WILAYA_TOTAL][TAILLE_WILAYA+1], int *taille){
    //printf("la fonction\n");
    char wilaya[TAILLE_WILAYA+1];
    char cle[TAILLE_CLE+1];
    int taille_obs;
    char superficie[TAILLE_SUPERFICIE + 1];
    char* observation;
    enr->effacer = 0;
    alea_cle(min, cle);
    strcpy(enr->cle, cle);
    *min =  atoi(enr->cle) + 1;
    //*min %= MAX_CLE;
    alea_wilaya(tab_wilaya,wilaya);
    strcpy(enr->wilaya,wilaya);
    enr->type = alea_type(); 
    alea_superficie(superficie);
    strcpy(enr->superficie,superficie);
    alea_observation(&observation, &taille_obs);
    enr->observation = malloc(sizeof(char) * (taille_obs+1));
    strcpy(enr->observation, observation);
    *taille = taille_obs + TAILLE_CLE + TAILLE_SUPERFICIE + TAILLE_TYPE + TAILLE_WILAYA + 1; // 1 pour le champ effacer 
    int l = *taille;
    char sortie[4];
    char taille_enr[4];
    int_to_char(l,sortie);
    //printf("%s\n",sortie);
    int i ;
    if ( strlen(sortie)<3 )
    {
        for (i = 0; i < 3-strlen(sortie); i++)
        {
            taille_enr[i] = '0';
        }
        taille_enr[i] = '\0';
        strcat(taille_enr,sortie);                
    }
    else{
        strcpy(taille_enr,sortie);
    }
    strcpy(enr->taille_enr,taille_enr);
}

// afficher entete
void afficher_entete(lovc* f){
    printf("entete.tete = %d\n",Entete_info(f,1));
    printf("entete.nb_ins = %d\n",Entete_info(f,2));
    printf("entete.nb_car_sup = %d\n",Entete_info(f,3));
    printf("entete.adr_dernier_bloc = %d\n",Entete_info(f,4));
    printf("entete.position_dns_dernier_bloc = %d\n",Entete_info(f,5));
    printf("entete.nbbloc = %d\n",Entete_info(f,6));
}
// aficher le ième bloc
void afficher_bloc(lovc* fich , int i ){
    maillon buf;
    LireDir(fich, i, &buf);
    if(strlen(buf.tab) == 0)
        printf("le buffers est vide \n");
    else{
        printf("%s \n",buf.tab);
    }
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
//remplire enete
void remplire_entete(lovc* f , Entete head){
    Aff_entete(f,1,head.tete);
    Aff_entete(f,2,head.nb_ins);
    Aff_entete(f,3,head.nb_car_sup);
    Aff_entete(f,4,head.adr_dernier_bloc);
    Aff_entete(f,5,head.position_dns_dernier_bloc);
    Aff_entete(f,6,head.nbbloc);
}

// ecrire enregistrement a la fin du fichier
void ecrire_enr_bloc(lovc* f , maillon* buf , enrengistrement enr){
    char copy[TAILLE_BLOC];
    enr_to_string(enr,copy);
    buf->suivant = -1;
    int i = Entete_info(f,4);
    int j = Entete_info(f,5);
    int k = 0;
    while (k < strlen(copy))
    {
        if(j < TAILLE_BLOC){
            buf->tab[j] = copy[k];
            j++;
            k++;
        }
        else{
            buf->tab[j] = '\0';
            buf->suivant = Entete_info(f,6) + 1;
            EcrireDir(f,Entete_info(f,4),buf);
            i = Entete_info(f,6) + 1;
            LireDir(f,i,buf);
            j = 0;
            Aff_entete(f,6,Entete_info(f,6)+1);
        }  
    }
    buf->tab[j]='\0';
    Aff_entete(f,2,Entete_info(f,2)+strlen(copy));
    Aff_entete(f,4,i);
    Aff_entete(f,5,j);
    EcrireDir(f,i,buf);
    FILE* fichier = f->fichier;
    fseek(fichier,0,SEEK_SET);
    fwrite(&(f->entete) , sizeof(Entete) , 1 , fichier);    
}
// creer fichier  Livrets_National.bin
lovc* creer_Livrets_National(char tab_wilaya[NB_WILAYA_TOTAL][TAILLE_WILAYA+1] , int* nb_enregistrement){
    lovc* f = Ouvrir(f,"Livrets_National.bin",'N');
    int nb = alea_nb(1,MAX_NB_LIVRETS_ALEATOIRES);
    printf("%d\n",nb);
    int min = 1;
    *nb_enregistrement = 0;
    enrengistrement enr;
    maillon buf;
    int taille;
    while (min<MAX_CLE && *nb_enregistrement < nb)
    {
        alea_enregistrement(&enr,&min,tab_wilaya,&taille);
        ecrire_enr_bloc(f,&buf,enr);
        *nb_enregistrement += 1;
    }
    fermer(f);    
    return f ;
}
// afficher le fichier  Livrets_National.bin
void afficher_Livrets_National(lovc* f){
    lovc* fich = Ouvrir(fich,"Livrets_National.bin",'A');
    printf("ouveert\n");
    afficher_entete(fich);
    for (int i = 0; i < Entete_info(fich,6); i++)
    {
        afficher_bloc(fich,i+1);
    }
    fermer(fich);
    
}

// recuperer chaine 
void recuperer_chaine(lovc* f , int* i , int* j , int taille , char chaine[TAILLE_BLOC]){
    maillon buf;
    LireDir(f,*i,&buf);
    int k = 0;
    while (k < taille)
    {
        if(*j < TAILLE_BLOC){
            chaine[k] = buf.tab[*j];
            *j += 1;
            k++;
        }
        else{
            *i = buf.suivant;
            LireDir(f,*i,&buf);
            *j = 0;
        }  
    }
}


int main(){
    srand(time(NULL));
    char tab_wilaya[NB_WILAYA_TOTAL][TAILLE_WILAYA+1];
    generer_tab_wilaya(tab_wilaya);
    int nb_enregistremen;
    lovc* f = creer_Livrets_National(tab_wilaya,&nb_enregistremen);
    afficher_Livrets_National(f);     
    return 0;
}