#include "types.h"
#include "moteur.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "GfxLib.h"
#include "ESLib.h"
#include "BmpLib.h"
#define LARGEUR_FENETRE 1500
#define HAUTEUR_FENETRE 800


#define MAX_AVIONS 50
#define NB_PISTES  3

// Logique
   // tableau de pointeurs

// Affichage (t, startX, startY)

void dessinesInformations(piste pistes[3],avionsListe* air, parking* prk);

extern int airW, airH;
extern int descW, descH;
extern int pisteW, pisteH;
extern int parkingW, parkingH;
extern int decollPisteW, decollPisteH;
extern int decollAirW, decollAirH;


void afficheImageReduite(float x, float y, DonneesImageRGB *img, int newW, int newH);
void initAeroport(void);
void dessineAeroport(void);
void dessineAvions(piste pistes[3],avionsListe* air, parking* prk);
void dessineAvions2(avion* av);
void dessineAvionForme(float x, float y);
void clicSouris(piste pistes[3],avionsListe* air, parking* prk);
void metAJourAnimation(piste pistes[3],avionsListe* air, parking* prk);
void metAJourAnimation2(piste pistes[3],avionsListe* air, parking* prk,avion* av);
bool pointDansBouton(const Bouton *b, int x, int y);
void gestionEvenement(EvenementGfx evenement);
