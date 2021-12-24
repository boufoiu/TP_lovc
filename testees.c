#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define TAILLE_BLOC 1       
#define TAILLE_WILAYA 14
#define TAILLE_SUPERFICIE 10
#define TAILLE_TYPE 1
#define TAILLE_CLE 10
#define MAX_SUPERFICIE 9999999999
#define MIN_SUPERFICIE 0
#define NB_WILAYA_TOTAL 58


//genere une observation aleatoirement               ***** marche *****     
                                                                                    
void alea_observation(char** obs){
    int min_taille = TAILLE_CLE+TAILLE_SUPERFICIE+TAILLE_TYPE+TAILLE_WILAYA;
    int taille = alea_nb(min_taille,TAILLE_BLOC);
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

// le teste
/*int main(){

   srand(time(NULL));
   char* obseration;
   alea_observation(&obseration);
   printf("%s \n",obseration);
   return 0;
}*/

///////////////////////////////////////////////////////////////////////////////////////::
// generer le tableau des wilaya 
void generer_tab_wilaya(char tab[NB_WILAYA_TOTAL][TAILLE_WILAYA+1] ){
   
    FILE* f = fopen("wilaya.txt","r");
    printf("la fonction \n");
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
// le teste 
/*int main(){
    //srand(time(NULL));
    char  tab_wilaya[NB_WILAYA_TOTAL][TAILLE_WILAYA+1] ;
    generer_tab_wilaya(tab_wilaya);
    for (int i = 0; i < NB_WILAYA_TOTAL; i++)
    {
        printf(" TTT %d  :  %s \n",i,tab_wilaya[i]);
    }
     
    
    return 0;
}*/

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////


// convertie un enregistrement a une chaine de caracteres 
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
// le teste
/*int main(){ 

    srand(time(NULL));
    enrengistrement info;
    info.observation = malloc(TAILLE_BLOC - sizeof(enrengistrement));
    info.observation = "i really the work you do";
    info.cle = 1000000440;
    info.effacer = 0;
    strcpy(info.superficie, "1236;**9");
    info.type = 'A';
    strcpy(info.wilaya, "Alger");

    char* string;
    enr_to_string(info, &string);
      
    printf("%s",string);

    return 0;
}*/
/////////////////////////////////////////////////////////////////:::::
/////////////////////////////////////////////////////////
//////////////////////////////////////////////////////:

// retourne une chaine pour le champs superficie     ***** marche ***** 
void alea_superficie(char* superficie){
    char s[TAILLE_SUPERFICIE+1];
    int_to_char(alea_nb(MIN_SUPERFICIE,MAX_SUPERFICIE),s);
    strcpy(superficie,s); 
}
// le teste
/*int main(){ 

    srand(time(NULL));
    char superficie[TAILLE_SUPERFICIE+1];
    alea_superficie(superficie);
    printf("%s \n",superficie);

    return 0;
}*/


///////////////////////////////////////////////////////:::
/////////////////////////////////////////////////:


// generer une wilaya aleatoirement
void alea_wilaya(char tab_wilaya[NB_WILAYA_TOTAL][TAILLE_WILAYA+1], char* wilaya){
    int i =alea_nb(0,NB_WILAYA_TOTAL-1);
    strcpy(wilaya,tab_wilaya[i]);
}

// le teste 
/*int main(){ 

    srand(time(NULL));
    char tab_wilaya[NB_WILAYA_TOTAL][TAILLE_WILAYA+1];
    generer_tab_wilaya(tab_wilaya);
    char wilaya[TAILLE_WILAYA+1];
    char a[TAILLE_WILAYA+1];
    alea_wilaya(tab_wilaya,wilaya);
    alea_wilaya(tab_wilaya,a);
    printf("%s \n", wilaya);
    printf("%s \n",a);

    return 0;
}*/
///////////////////////////////////////////////////////////////////:
////////////////////////////////////////////::

// generer un enregistrement aleatoire
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



/*int main(){ 
    srand(time(NULL));
    char tab_wilaya[NB_WILAYA_TOTAL][TAILLE_WILAYA+1];
    generer_tab_wilaya(tab_wilaya);
    enrengistrement enr;
    alea_enregistrement(&enr,0,tab_wilaya);
    char* copy;
    enr_to_string(enr,&copy);
    printf("%s \n",copy);

    return 0;
}
*/