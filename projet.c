#include "moteur.h"
#include "affichage.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

parking initParking(int id, int nbMax){
    parking p;
    p.id = id;
    p.nbMax = nbMax;
    p.avions= initAvionsListe();
    return p;
}

piste initPiste(int id, int longueur, enum categoriePiste cat, int nbMax){
    piste pst;
    pst.id = id;
    pst.longueur = longueur;
    pst.idAvion = -1;
    pst.categorie = cat;
    pst.etat = libre;
    pst.nbMax = nbMax;
    pst.avions= initAvionsListe();
    return pst;
}

avion* initAvion(int id, enum categorieAvion cat, enum etatAvion et, int nbPass,int emplacement,int startX, int startY,int idPiste){ //SI c'est la toute première fois qu'on initialise a 0 startX et startY sinon il faut regarder dans la base de donnés et mettre la même valeur
    avion* a = malloc(sizeof(avion));
    if (!a) return NULL;
    a->id = id;
    a->categorie = cat;
    a->etat = et;
    a->nbPassager = nbPass;
    a->avionSuiv = NULL;
    a->t = 0.f;
    a->startX = startX;
    a->startY = startY;
    a->emplacement=emplacement;
    a->idPiste = idPiste;
    return a;
}

avionsListe initAvionsListe(void){
    avionsListe a;
    a.nbActuelle = 0;
    a.premierAvion= NULL;
    return a;
}

avionsListe delAvionListeId(avionsListe avions,int id){ //si il y a un problème changer le return  | cette fonction sert pour la liste d'avions parking et la liste d'avions dans les airs | La fonction sert a supprimer l'avion dans la liste 
    if (avions.premierAvion ==NULL){
        printf("Liste vide \n");
		return avions;
	}
    avion* ptliste = avions.premierAvion;
    if (avions.premierAvion->id==id){
        ptliste = ptliste->avionSuiv;
        avions.premierAvion =ptliste;
        avions.nbActuelle--;
        return avions;
    }
    if (ptliste->avionSuiv== NULL){ 
        printf("Pas trouvé\n");
        return avions;
    }
	while(ptliste->avionSuiv->avionSuiv!=NULL && ptliste->avionSuiv->id!=id){
		ptliste = ptliste->avionSuiv;		
	}
    if (ptliste->avionSuiv->id==id){
        ptliste->avionSuiv=ptliste->avionSuiv->avionSuiv;
        avions.nbActuelle--;
        return avions;
    }
    printf("Pas trouvé\n");
    return avions;
}

avionsListe addAvionListe(avionsListe avions,avion* newAvion){
    newAvion->avionSuiv = avions.premierAvion;
    avions.premierAvion = newAvion;
    avions.nbActuelle++;
    return avions;    
}


avionsListe delAvionListe(avionsListe avions){
    avion* ptr = avions.premierAvion;
    if (ptr==NULL){
        return avions;
    }
    if (ptr->avionSuiv==NULL){
        avions.premierAvion =NULL;  
        avions.nbActuelle--;
        return avions;
    }
    while (ptr->avionSuiv->avionSuiv!= NULL){
        ptr = ptr->avionSuiv;
    }
    avions.nbActuelle--;
    ptr->avionSuiv = NULL;
    //free(ptr->avionSuiv); normalement pas besoin car on le stock après
    return avions;
}

void afficherAvions(avion* liste) {
    avion* a = liste;
    while (a != NULL) {
        printf("ID: %d | Catégorie: %s | Etat: %s | Passagers: %d\n",
               a->id,
               categorieAvionToString(a->categorie),
               etatAvionToString(a->etat),
               a->nbPassager);
        a = a->avionSuiv;
    }
}
void afficherEtatAeroport(parking* prk, piste pistes[3], avionsListe* air) {
    if (!prk || !air) return;

    printf("=== État du Parking (ID %d) nombre avion : %d  ===\n", prk->id, prk->avions.nbActuelle);
    if (prk->avions.nbActuelle == 0) {
        printf("Aucun avion dans le parking.\n");
    } else {
        afficherAvions(prk->avions.premierAvion);
    }

    for (int i = 0; i < 3; i++) {
        printf("\n=== État de la Piste %d (ID %d) ===\n", i, pistes[i].id);
        printf("Catégorie: %s | État: %s | Nb avions: %d\n",
               pistes[i].categorie == petite ? "petite" :
               pistes[i].categorie == moyenne ? "moyenne" : "grande",
               pistes[i].etat == libre ? "libre" :
               pistes[i].etat == pisteDecollage ? "pisteDecollage" : "pisteAtterissage",
               pistes[i].avions.nbActuelle);
        if (pistes[i].avions.nbActuelle == 0) {
            printf("Aucun avion sur cette piste.\n");
        } else {
            afficherAvions(pistes[i].avions.premierAvion);
        }
    }

    printf("\n=== Avions dans les airs ===\n");
    if (air->nbActuelle == 0) {
        printf("Aucun avion en vol.\n");
    } else {
        afficherAvions(air->premierAvion);
    }

    printf("\n============================\n");
}




enum categorieAvion stringToCategorieAvion(const char* s) {
    if (strcmp(s, "CAT_ligne") == 0) return CAT_ligne;
    if (strcmp(s, "affaire") == 0) return affaire;
    if (strcmp(s, "leger") == 0) return leger;
    return leger; // défaut
}

enum etatAvion stringToEtatAvion(const char* s) {
    if (strcmp(s, "vol") == 0) return vol;
    if (strcmp(s, "sol") == 0) return sol;
    if (strcmp(s, "decollage") == 0) return decollage;
    if (strcmp(s, "atterisage") == 0) return atterisage;
    if (strcmp(s, "decPiste") == 0) return decPiste;
    if (strcmp(s, "attPiste") == 0) return attPiste;
    if (strcmp(s, "waitPiste") == 0) return waitPiste;

    return sol; // défaut
}


int loadParking(parking* p, const char* nomFichier) {
    if (!p) return 0;

    FILE* f = fopen(nomFichier, "r");
    if (!f) {
        printf("Erreur ouverture fichier %s\n", nomFichier);
        return 0;
    }

    // Lecture de l’en-tête
    char header[64];
    int id, nbMax;

    if (fscanf(f, "%[^;];%d;%d\n", header, &id, &nbMax) != 3) {
        fclose(f);
        return 0;
    }

    // Initialisation du parking
    *p = initParking(1, 8);
    // Lecture des avions
    while (!feof(f)) {
        int idA, nbPass, emplacement, idPiste;
        char catStr[64], etatStr[64];
        float startX, startY, t;

        int ok = fscanf(f, "%d;%[^;];%[^;];%d;%d;%d;%f;%f;%f\n",
                        &idA, catStr, etatStr, &nbPass, &emplacement,
                        &idPiste, &startX, &startY, &t);

        if (ok == 9) {
            avion* a = initAvion(idA, stringToCategorieAvion(catStr),
                                 stringToEtatAvion(etatStr), nbPass,
                                 emplacement, startX, startY, idPiste);
            a->t = t;

            // Ajout en tête de liste
            a->avionSuiv = p->avions.premierAvion;
            p->avions.premierAvion = a;
            p->avions.nbActuelle++;
        }
    }

    fclose(f);
    return 1;
}


int loadPiste(piste* pst, const char* nomFichier) {
    if (!pst) return 0;

    FILE* f = fopen(nomFichier, "r");
    if (!f) {
        printf("Erreur ouverture fichier %s\n", nomFichier);
        return 0;
    }

    // Lecture entête piste
    char header[64], catStr[64], etatStr[64];
    int id, longueur, nbMax;

    if (fscanf(f, "%[^;];%d;%d;%[^;];%[^;];%d\n",
               header, &id, &longueur, catStr, etatStr, &nbMax) != 6) {
        fclose(f);
        return 0;
    }

    // Catégorie piste
    enum categoriePiste cat = 
        strcmp(catStr, "petite") == 0 ? petite :
        strcmp(catStr, "moyenne") == 0 ? moyenne : grande;

    // État piste
    enum pisteEtat et =
        strcmp(etatStr, "libre") == 0 ? libre :
        strcmp(etatStr, "pisteDecollage") == 0 ? pisteDecollage : pisteAtterissage;

    *pst = initPiste(id, longueur, cat, nbMax);
    pst->etat = et;

    // Lecture avions (comme parking)
    while (!feof(f)) {
        int idA, nbPass, emplacement, idPiste;
        char catA[64], etatA[64];
        float startX, startY, t;

        int ok = fscanf(f, "%d;%[^;];%[^;];%d;%d;%d;%f;%f;%f\n",
                        &idA, catA, etatA, &nbPass, &emplacement,
                        &idPiste, &startX, &startY, &t);

        if (ok == 9) {
            avion* a = initAvion(idA, stringToCategorieAvion(catA),
                                 stringToEtatAvion(etatA), nbPass,
                                 emplacement, startX, startY, idPiste);
            a->t = t;

            a->avionSuiv = pst->avions.premierAvion;
            pst->avions.premierAvion = a;
            pst->avions.nbActuelle++;
        }
    }

    fclose(f);
    return 1;
}


int loadAir(avionsListe* air, const char* nomFichier) {
    if (!air) return 0;

    FILE* f = fopen(nomFichier, "r");
    if (!f) {
        printf("Erreur ouverture fichier %s\n", nomFichier);
        return 0;
    }

    // Réinitialisation
    air->nbActuelle = 0;
    air->premierAvion = NULL;

    while (!feof(f)) {
        int idA, nbPass, emplacement, idPiste;
        char catStr[64], etatStr[64];
        float startX, startY, t;

        int ok = fscanf(f, "%d;%[^;];%[^;];%d;%d;%d;%f;%f;%f\n",
                        &idA, catStr, etatStr, &nbPass,
                        &emplacement, &idPiste, &startX, &startY, &t);

        if (ok != 9) continue;

        avion* a = initAvion(idA, stringToCategorieAvion(catStr),
                             stringToEtatAvion(etatStr), nbPass,
                             emplacement, startX, startY, idPiste);
        a->t = t;

        // Insertion en tête
        a->avionSuiv = air->premierAvion;
        air->premierAvion = a;
        air->nbActuelle++;
    }

    fclose(f);
    return 1;
}

int dataBaseParking(parking* p, const char nomFichier[]){
    if (!p) return 0;
    FILE* f = fopen(nomFichier, "w");
    if (!f){
        printf("erreur ouverture fichier %s\n", nomFichier);
        return 0;
    }

    fprintf(f, "Parking;%d;%d\n", p->id, p->nbMax);

    // écriture inversée
    ecrireAvionsInverse(f, p->avions.premierAvion);

    fclose(f);
    return 1;
}

int dataBaseAvion(avion* liste, const char nomFichier[]) {
    FILE* f = fopen(nomFichier, "w");
    if (!f) {
        printf("Erreur ouverture fichier %s\n", nomFichier);
        return 0;
    }

    // écriture inversée
    ecrireAvionsInverse(f, liste);

    fclose(f);
    return 1;
}

int dataBasePiste(piste* pst, const char nomFichier[]) {
    if (!pst) return 0;

    FILE* f = fopen(nomFichier, "w");
    if (!f) {
        printf("Erreur ouverture fichier %s\n", nomFichier);
        return 0;
    }

    fprintf(f, "Piste;%d;%d;%s;%s;%d\n",
            pst->id,
            pst->longueur,
            (pst->categorie == petite ? "petite" :
             pst->categorie == moyenne ? "moyenne" : "grande"),
            (pst->etat == libre ? "libre" :
             pst->etat == pisteDecollage ? "pisteDecollage" : "pisteAtterissage"),
            pst->nbMax);

    // écriture inversée
    ecrireAvionsInverse(f, pst->avions.premierAvion);

    fclose(f);
    return 1;
}

void ecrireAvionsInverse(FILE* f, avion* premier) {
    // Compter les avions
    int n = 0;
    avion* tmp = premier;
    while (tmp) {
        n++;
        tmp = tmp->avionSuiv;
    }

    // Stockage dans un tableau
    avion** tab = malloc(n * sizeof(avion*));
    tmp = premier;
    for (int i = 0; i < n; i++) {
        tab[i] = tmp;
        tmp = tmp->avionSuiv;
    }

    // Écriture en ordre inversé
    for (int i = n-1; i >= 0; i--) {
        avion* a = tab[i];
        fprintf(f, "%d;%s;%s;%d;%d;%d;%f;%f;%f\n",
                a->id,
                categorieAvionToString(a->categorie),
                etatAvionToString(a->etat),
                a->nbPassager,
                a->emplacement,
                a->idPiste,
                a->startX,
                a->startY,
                a->t);
    }

    free(tab);
}






const char* categorieAvionToString(enum categorieAvion cat) {
    switch(cat) {
        case CAT_ligne: return "CAT_ligne";
        case affaire: return "affaire";
        case leger: return "leger";
        default: return "inconnu";
    }
}

const char* etatAvionToString(enum etatAvion e) {
    switch(e) {
        case vol: return "vol";
        case sol: return "sol";
        case decollage: return "decollage";
        case atterisage: return "atterisage";
        case decPiste: return "decPiste";
        case attPiste: return "attPiste";
        case waitPiste: return "waitPiste";
        default: return "inconnu";
    }
}

bool pisteLibre(const piste pst){
    return (pst.etat == libre);
}


avion* getlastAvion(avionsListe liste){
    avion* tmp = liste.premierAvion;
    if(tmp == NULL){
        printf("il n'y a pas d'avion en dernier");
        return NULL;

    }
    while(tmp->avionSuiv != NULL){
        tmp = tmp->avionSuiv;
    }
    return tmp;

}

avion* getlasttypeAvion(avionsListe liste,enum categorieAvion type){
    avion* tmp = liste.premierAvion;
    avion* ptr = NULL;
    if(tmp == NULL){
        printf("il n'y a pas d'avions");
        return NULL;

    }
    while(tmp != NULL){
        if (tmp->categorie==type){
            ptr =tmp;
        }
        tmp = tmp->avionSuiv;
    }
    return ptr;

}

avion* findAvion(avionsListe liste,int id){
    avion* tmp = liste.premierAvion;
    while(tmp!= NULL){
        if (tmp->id==id){
            return tmp;
        }
        tmp = tmp->avionSuiv;
    }
    printf("il n'y a pas d'avions avec cette id");
    return NULL;
}

void setEmplacementPiste(piste* pst){
    if (pst ==NULL){
        return;
    }
    avion* tmp = pst->avions.premierAvion;
    while(tmp->avionSuiv!= NULL){
        tmp->emplacement--;
        tmp =tmp->avionSuiv;
    }
    tmp->etat=decPiste;
}

void ajouteEmplacementPrk(avionsListe prk,avion* av){
    avion* tmp;
    for (int i =0; i<8;i++){ //Il y a que 8 place dans le parkings
        tmp = prk.premierAvion;
        while(tmp!= NULL){
            if (tmp->emplacement==i){ //L'emplacement est deja pris on sort pour regarder le prochain
                break;
            }
            tmp =tmp->avionSuiv;
        }
        if (tmp == NULL){
            av->emplacement =i; 
            break;           
        }
    }
}

void ajouteEmplacement(avion* premierAv){
    avion* tmp = premierAv;
    tmp->emplacement = -1;
    while(tmp!= NULL){
        tmp->emplacement++;
        tmp =tmp->avionSuiv;
    }
}

void decolagePisteAttente(piste* pst,avionsListe* air, parking* prk){
	if (pisteLibre(*pst) == true){
		if (pst->avions.nbActuelle >0){
            pst->etat = pisteDecollage;
            setEmplacementPiste(pst);  
        }  
    }
}

int decolage(piste* pst,avionsListe* air, parking* prk,avion* newavion){ // ON doit faire un for pour chaque pst
	if (pisteLibre(*pst) == true){
		if (pst->avions.nbActuelle < pst->nbMax){
            if (newavion != NULL){
                prk->avions = delAvionListeId(prk->avions,newavion->id);                
                pst->avions = addAvionListe(pst->avions, newavion); //a ajouter pour la piste
                newavion->emplacement = pst->avions.nbActuelle+1;
                newavion->idPiste = pst->id; 
                newavion->etat = waitPiste;
                newavion->t = 0.f;   
            }
        }
        pst->etat = pisteDecollage;
        setEmplacementPiste(pst);
        return 1;
    }	
    else{
        if (pst->avions.nbActuelle<pst->nbMax){
            prk->avions = delAvionListeId(prk->avions,newavion->id);                
            pst->avions = addAvionListe(pst->avions, newavion); //a ajouter pour la piste
            newavion->emplacement = pst->avions.nbActuelle-1;
            newavion->idPiste = pst->id;
            newavion->etat = waitPiste;
            newavion->t = 0.f;
            return 1;
        }
        return 0;
    }

}

int atterit(piste pistes[3],avionsListe* air, parking* prk,avion* newavion){ //Si ca renvoie 1 alors tout c'est bien passé si ca renvoie 0 la piste est complète donc on essaye de faire une autre piste
    piste* pst = &pistes[newavion->categorie];
    printf("non");
    int x;
    int ajout=0; //nombre d'avions qui ont une place reservé dans le parking
    for (int i =0;i<3;i++){
        if (pistes[i].etat==pisteAtterissage){
            ajout++;
        }
    }
    if (pisteLibre(*pst) == true){
        if (prk->avions.nbActuelle+ajout<prk->nbMax){
            *air = delAvionListeId(*air,newavion->id);
            pst->avions = addAvionListe(pst->avions,newavion);
            pst->etat = pisteAtterissage;
            pst->idAvion = newavion->id;

            float largeurAffichee = (float) airW;
            float espacement = largeurAffichee + 10.0f;
            newavion->startX = 50.0f + newavion->emplacement * espacement;
            newavion->startY = HAUTEUR_FENETRE - 150.0f;
            newavion->etat = atterisage;
            newavion->idPiste = newavion->categorie;
            newavion->t = 0.f;
            newavion->emplacement=pst->avions.nbActuelle;
        }
        else{
            x= realisesDecollage(pistes,air,prk,newavion->categorie);
            if (x==1){
                *air = delAvionListeId(*air,newavion->id);
                pst->avions = addAvionListe(pst->avions,newavion);
                pst->etat = pisteAtterissage;
                pst->idAvion = newavion->id;

                float largeurAffichee = (float) airW;
                float espacement = largeurAffichee + 10.0f;
                newavion->startX = 50.0f + newavion->emplacement * espacement;
                newavion->startY = HAUTEUR_FENETRE - 150.0f;
                newavion->etat = atterisage;
                newavion->idPiste = newavion->categorie;
                newavion->t = 0.f;
                newavion->emplacement=pst->avions.nbActuelle;
            }
            else{
                return 0;
            }
        }
    }
    else{
        return 0;
    }
    return 1;
}

int realisesDecollage(piste pistes[3],avionsListe* air, parking* prk,int prio){ 
    avion* newavion;
    int x = 0;
    printf("oui\n");
    for (int i=0;i<3;i++){
        if (prio!=i){  // Si on a pas deja fait cette catégorie
            printf("vrmnt\n");
            newavion = getlasttypeAvion(prk->avions,i); 
            printf("surement\n");
            if (newavion!=NULL){ //Si c'est égale a NULL  il n'y a pas d'avions de ce type dans les airs
                printf("je passe pas\n");                 
                x= decolage(&pistes[i],air,prk,newavion);   
                printf("je passe\n");                 
            }
            if (x ==1){
                printf("j'ai retutn\n");
                return x;
            }

        }
    }
    printf("fini\n");
    newavion = getlasttypeAvion(prk->avions,prio); 
    if (newavion!=NULL){ //Si c'est égale a NULL  il n'y a pas d'avions de ce type dans les airs
        x = decolage(pistes,air,prk,newavion);                    
    }
    if (x==1){
        x++;
    }
    return x;
}

void realisesAtterrit(piste pistes[3],avionsListe* air, parking* prk){
    avion* lastAvion = getlastAvion(*air);
    printf("ici1\n");
    if (lastAvion ==NULL){
        return;
    }
    int x = atterit(pistes,air, prk,lastAvion);
    printf("ici2\n");
    if (x==0){
        avion* newavion;
        for (int i=0;i<3;i++){
            if (lastAvion->categorie!=i){  // Si on a pas deja fait cette catégorie
                newavion = getlasttypeAvion(*air,i); 
                printf("ici3\n");
                if (newavion!=NULL){ //Si c'est égale a NULL  il n'y a pas d'avions de ce type dans les airs
                    x = atterit(pistes,air,prk,newavion);  
                    if (x==1){
                        break;
                    }  
                }

            }
        }
    }
}

void libereTout(piste pistes[3], avionsListe* air, parking* prk)
{
    for (int i =0;i<3;i++){
        pistes[i].avions = libere(pistes[i].avions);
    }
    *air = libere(*air);
    prk->avions = libere(prk->avions);
}

avionsListe libere(avionsListe file){
    avion* tmp = file.premierAvion;
    while(tmp!=NULL){
        file.premierAvion = file.premierAvion->avionSuiv;
        free(tmp);
        tmp = file.premierAvion;
    }
    file.nbActuelle=0;
    return file;
}