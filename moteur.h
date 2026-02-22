#ifndef moteur
#define moteur
#include "types.h"
#include <stdbool.h>
#include <stdio.h>   
// parking initparking();
// piste initpiste();
// avion initavion();
void ecrireAvionsInverse(FILE* f, avion* premier);
void libereTout(piste pistes[3], avionsListe* air, parking* prk);
avionsListe libere(avionsListe file);
parking initParking(int id, int nbMax);
piste initPiste(int id, int longueur, enum categoriePiste cat, int nbMax);
avion* initAvion(int id, enum categorieAvion cat, enum etatAvion et, int nbPass,int emplacement,int startX, int startY,int idPiste);
void decolagePisteAttente(piste* pst,avionsListe* air, parking* prk);
avionsListe addAvionListe(avionsListe avions, avion* newAvion);
avionsListe delAvionListe(avionsListe avions);
avionsListe delAvionListeId(avionsListe avions, int id);

avion* getlastAvion(avionsListe liste);
avion* getlasttypeAvion(avionsListe liste, enum categorieAvion type);
avion* findAvion(avionsListe liste, int id);

enum categorieAvion stringToCategorieAvion(const char* s);
enum etatAvion stringToEtatAvion(const char* s);
const char* categorieAvionToString(enum categorieAvion cat);
const char* etatAvionToString(enum etatAvion e);

int dataBaseParking(parking* p, const char nomFichier[]);
int dataBasePiste(piste* pst, const char nomFichier[]);
int dataBaseAvion(avion* liste, const char nomFichier[]);

int loadAvions(const char* nomFichier,piste pistes[3] ,avionsListe* air,parking* prk);
// Affichage
void afficherAvions(avion* liste);
avionsListe initAvionsListe(void);
void ajouteEmplacement(avion* premierAv);
void ajouteEmplacementPrk(avionsListe prk,avion* av);
// État piste

bool pisteLibre(const piste pst);
void setEmplacementPiste(piste* pst);
int decolage(piste* pst, avionsListe* air, parking* prk, avion* newavion);
int atterit(piste pistes[3], avionsListe* air, parking* prk, avion* newavion);
int realisesDecollage(piste pistes[3], avionsListe* air, parking* prk, int prio);
void realisesAtterrit(piste pistes[3], avionsListe* air, parking* prk);
int loadParking(parking* p, const char* nomFichier);
int loadPiste(piste* pst, const char* nomFichier);
int loadAir(avionsListe* air, const char* nomFichier);
void afficherEtatAeroport(parking* prk, piste pistes[3], avionsListe* air);
//avionsListe initavionsListe();

#endif