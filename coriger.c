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
// Ouvrire un fichier lovc 
lovc* Ouvrir(lovc* f, char nom[], char mode){
    maillon buf;
    lovc *fichier = malloc(sizeof(lovc));                                                                    
    if (mode == 'A' || mode == 'a')
    { 
        f->fichier = fopen(nom,"rb+");                                      // ouverture du fichier en mode binaire lecture et ecriture
        fread(&(f->entete),sizeof(Entete),1,fichier->fichier);             // chargement de l'entete enregistree en debut de fichier
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
    char cle[TAILLE_CLE+1];
    strcpy(string, info.cle);
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

// a+(rand())%(b-a) 
/***     $ : entre deux champs     @  : entre deux livrets
 *  les fonctions a programmer sont :
 *          . int alea_nb(int a , int b) // retourne un nembre aleatoire entre a et b                    ****
 *          . char* alea_wilaya() // retourne le nom d'une wilaya aleatoirement
 *          . char* alea_superficie() // retourne une chaine pour le champs superficie


*/
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
//generer une observation aleatoirement                                      ***** marche *****     
void alea_observation(char** obs,int* longueur){
    int min_taille = TAILLE_CLE+TAILLE_SUPERFICIE+TAILLE_TYPE+TAILLE_WILAYA;
    *longueur = alea_nb(min_taille,TAILLE_BLOC-min_taille-2);
    int taille = *longueur;
    //printf("taille  = %d\n",taille); 
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
    printf("la fonction\n");
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
    enr->effacer = 0;
}

// afficher entete
void afficher_entete(lovc* f){
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
    LireDir(fich, i, &buf);
    if(strlen(buf.tab) == 0)
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



int main(){
    srand(time(NULL));
    char *obs, tab[NB_WILAYA_TOTAL][TAILLE_WILAYA], *str, *test, koko[TAILLE_CLE];
    enrengistrement enr;
    int nombre = 12, tailleblock = 0;
    generer_tab_wilaya(tab);
    alea_enregistrement(&enr,&nombre, tab, &tailleblock);
    printf("la taille du enr est: %d\n",tailleblock);
    str = malloc(sizeof(char)* tailleblock);
    /* printf("observation = %s\n", enr.observation);
    printf("cle = %s\n", enr.cle);
    printf("superficie = %s\n", enr.superficie);
    printf("wilaya = %s\n", enr.wilaya); */

    enr_to_string(enr, &str);
    printf("%s\n", str);
   /*  printf("effacer = %d",enr.effacer);
    printf("the size of the enr est: %d",tailleblock); */
    for (int i = 0; i < 5; i++)
    {
        printf("i = %d",i);
        printf("obs %d\n", enr.observation);
        free(*enr.observation);
        
        free(str);
        
        alea_enregistrement(&enr,&nombre, tab, &tailleblock);
        printf("la taille du enr est: %d\n",tailleblock);
        str = malloc(sizeof(char)* tailleblock);
        enr_to_string(enr, &str);
        printf("%s\n", str);
        
    }
    


    

    /* alea_observation(str, &taille);
    test = malloc(sizeof(char) * taille);
    strcpy(test, str);
    printf("the observation is : %s\n", test);
    printf("la taille de l'observation est: %d",taille);  */   
    
    return 0;
}
/* void generer_livret(char RES[100]) // generer un livre avec tous ses champs
{
char R[100]={'0'};
char wilaya[15];
char num[11];
char observ[50]; char ch[3];
char super[11]; char tab[3];
int i,nb,taille; char type;


generer_observation(observ); //générer une observation pour le livret du taille variable
taille=strlen(observ)+39; //calcule de la taille du livret qui est egale à la la taille de tous ses champs du taille fixe=39+ la taille de l'observation
nb=nb_pos(taille); //on doit savoir le nombre de position dela taille pour la mettre sur 3
for (i=0 ;i<3-nb ;i++){
tab[i]='0';
}
tab[i]='\0';
itoa(taille,ch,10); //convertir la taille en chaine de caractere pour pouvoir la concatiner
strcat(tab,ch); //concatiner la taille avec des '0' pour qu'elle soit sur 3 caractéres

strcpy(R,tab); //copier la taille calculée dans la chaine R qui est initialisé à '0' , car c'est le premier champs dans le livret
R[3]='0';



// mettre le champs (effacé) qui est dans la position 3 à 0 car le livret n'est pas encore supprimé
generer_numero(num); //on génére un numero pour le livret
strcat(R,num); // mettre le num dans la chaine R
generer_wilaya(wilaya);// generer une wilaya et la mettre dans la chaine aussi
strcat(R,wilaya);
type=generer_type(3); //générer un type qui sera sur un seul caractère, puis le stocker dans sa bonne position (28)
R[28]=type;
generer_superficie(super); // on génére une superficie pour le livret
strcat(R,super);
strcat(R,observ); //finalement on concatène l'observation, la superficie avec le rest de la chaine R qui contient tous les informations du livret
R[strlen(R)]='\0'; // pour indiquer la fin du la chaine

// copier la chaine R dans le string RES qui est le resultat cad le livret généré aléatoirement .
strcpy(RES,R);

} */
