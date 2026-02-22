#include "affichage.h"
#include "moteur.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "GfxLib.h"
#include "ESLib.h"
#include "BmpLib.h"



float hauteurPistes[NB_PISTES];
float centrePistes[NB_PISTES];


// Avions de ligne (avion1*.bmp)
DonneesImageRGB *imgLigneAir          = NULL; // avion1.bmp
DonneesImageRGB *imgLigneDesc         = NULL; // avion1a.bmp
DonneesImageRGB *imgLignePiste        = NULL; // avion1ap.bmp
DonneesImageRGB *imgLigneParking      = NULL; // avion1.bmp (même image que vol)
DonneesImageRGB *imgLigneDecollPiste  = NULL; // avion1dp.bmp
DonneesImageRGB *imgLigneDecollAir    = NULL; // avion1d.bmp

// Avions légers (avion2*.bmp)
DonneesImageRGB *imgLegerAir          = NULL; // avion2.bmp
DonneesImageRGB *imgLegerDesc         = NULL; // avion2a.bmp
DonneesImageRGB *imgLegerPiste        = NULL; // avion2ap.bmp
DonneesImageRGB *imgLegerParking      = NULL; // avion2.bmp
DonneesImageRGB *imgLegerDecollPiste  = NULL; // avion2dp.bmp
DonneesImageRGB *imgLegerDecollAir    = NULL; // avion2d.bmp

// Avions d’affaire (avion3*.bmp)
DonneesImageRGB *imgAffAir            = NULL; // avion3.bmp
DonneesImageRGB *imgAffDesc           = NULL; // avion3a.bmp
DonneesImageRGB *imgAffPiste          = NULL; // avion3ap.bmp
DonneesImageRGB *imgAffParking        = NULL; // avion3.bmp
DonneesImageRGB *imgAffDecollPiste    = NULL; // avion3dp.bmp
DonneesImageRGB *imgAffDecollAir      = NULL; // avion3d.bmp

Bouton boutonAtterrissage;
Bouton boutonDecollage;

int airW = 0, airH = 0;
int descW = 0, descH = 0;
int pisteW = 0, pisteH = 0;
int parkingW = 0, parkingH = 0;
int decollPisteW = 0, decollPisteH = 0;
int decollAirW = 0, decollAirH = 0;


void afficheImageReduite(float x, float y, DonneesImageRGB *img, int newW, int newH)
{
    if (img == NULL || newW <= 0 || newH <= 0)
        return;

    int w = img->largeurImage;
    int h = img->hauteurImage;

    for (int j = 0; j < newH; j++)
    {
        int srcY = (int)((float)j * h / newH);
        for (int i = 0; i < newW; i++)
        {
            int srcX = (int)((float)i * w / newW);

            unsigned char r = img->donneesRGB[3 * (srcY * w + srcX) + 0];
            unsigned char g = img->donneesRGB[3 * (srcY * w + srcX) + 1];
            unsigned char b = img->donneesRGB[3 * (srcY * w + srcX) + 2];

            couleurCourante(r, g, b);
            point(x + i, y + j);
        }
    }
}

//

int main(int argc, char **argv)
{
    initialiseGfx(argc, argv);
    prepareFenetreGraphique("Simu aeroport", LARGEUR_FENETRE, HAUTEUR_FENETRE);
    lanceBoucleEvenements();
    return 0;
}

void gestionEvenement(EvenementGfx evenement)
{
    static avionsListe air;
    static piste pistes[3];
    static parking prk;
    switch (evenement)
    {
    case Initialisation:
        if (loadAir(&air,"air.txt") == 0){ //Le fichier s'appelle avion.txt pour l'instant
            exit(1);
        }
        if (loadParking(&prk,"parking.txt") == 0){ //Le fichier s'appelle avion.txt pour l'instant
            exit(1);
        }
        if (loadPiste(&pistes[0],"piste1.txt") == 0){ //Le fichier s'appelle avion.txt pour l'instant
            exit(1);
        }
        if (loadPiste(&pistes[1],"piste2.txt") == 0){ //Le fichier s'appelle avion.txt pour l'instant
            exit(1);
        }
        if (loadPiste(&pistes[2],"piste3.txt") == 0){ //Le fichier s'appelle avion.txt pour l'instant
            exit(1);
        }
        initAeroport();
        demandeTemporisation(10);   // timer ~33 fps

        break;

    case Affichage:
        dessineAeroport();
        dessineAvions(pistes,&air,&prk);
        //dessinesInformations(pistes,&air,&prk); ca bug quand on met cette fonction donc on l'enlève
        break;

    case BoutonSouris:
        clicSouris(pistes,&air,&prk); //j pour jeu
        break;

    case Temporisation:
        for (int i =0;i<3;i++){
            decolagePisteAttente(&pistes[i],&air,&prk);
        }
        metAJourAnimation(pistes,&air,&prk);
        afficherEtatAeroport(&prk,pistes,&air);
        rafraichisFenetre();
        break;

    case Redimensionnement:

    case Clavier:
         char c = caractereClavier();
            if (c == 27) { // 27 = ESC
                dataBasePiste(&pistes[0],"piste1.txt");
                dataBasePiste(&pistes[1],"piste2.txt");
                dataBasePiste(&pistes[2],"piste3.txt");
                dataBaseParking(&prk,"parking.txt");
                dataBaseAvion(air.premierAvion,"air.txt");
                printf("jelibretout\n");
                libereTout(pistes,&air,&prk);
                termineBoucleEvenements(); // ferme la fenêtre proprement
            }
            break;

    case ClavierSpecial:
    case Souris:
    case Inactivite:
    default:
        break;
    }
}


void initAeroport(void)
{
    // Boutons
    float marge = 40.f;
    float largeurBouton = 200.f;
    float hauteurBouton = 50.f;

    boutonAtterrissage.x1 = marge;
    boutonAtterrissage.y1 = HAUTEUR_FENETRE - marge - hauteurBouton;
    boutonAtterrissage.x2 = boutonAtterrissage.x1 + largeurBouton;
    boutonAtterrissage.y2 = boutonAtterrissage.y1 + hauteurBouton;

    boutonDecollage.x1 = boutonAtterrissage.x2 + marge;
    boutonDecollage.y1 = boutonAtterrissage.y1;
    boutonDecollage.x2 = boutonDecollage.x1 + largeurBouton;
    boutonDecollage.y2 = boutonAtterrissage.y2;

    imgLigneAir         = lisBMPRGB("avion1.bmp");
    imgLigneDesc        = lisBMPRGB("avion1a.bmp");
    imgLignePiste       = lisBMPRGB("avion1ap.bmp");
    imgLigneDecollPiste = lisBMPRGB("avion1dp.bmp");
    imgLigneDecollAir   = lisBMPRGB("avion1d.bmp");
    imgLigneParking     = lisBMPRGB("avion1.bmp");

    if (!imgLigneAir)         printf("Erreur : impossible de charger avion1.bmp\n");
    if (!imgLigneDesc)        printf("Erreur : impossible de charger avion1a.bmp\n");
    if (!imgLignePiste)       printf("Erreur : impossible de charger avion1ap.bmp\n");
    if (!imgLigneDecollPiste) printf("Erreur : impossible de charger avion1dp.bmp\n");
    if (!imgLigneDecollAir)   printf("Erreur : impossible de charger avion1d.bmp\n");
    if (!imgLigneParking)     printf("Erreur : impossible de charger avion1.bmp (parking)\n");

    imgLegerAir         = lisBMPRGB("avion2.bmp");
    imgLegerDesc        = lisBMPRGB("avion2a.bmp");
    imgLegerPiste       = lisBMPRGB("avion2ap.bmp");
    imgLegerDecollPiste = lisBMPRGB("avion2dp.bmp");
    imgLegerDecollAir   = lisBMPRGB("avion2d.bmp");
    imgLegerParking     = lisBMPRGB("avion2.bmp");

    if (!imgLegerAir)         printf("Erreur : impossible de charger avion2.bmp\n");
    if (!imgLegerDesc)        printf("Erreur : impossible de charger avion2a.bmp\n");
    if (!imgLegerPiste)       printf("Erreur : impossible de charger avion2ap.bmp\n");
    if (!imgLegerDecollPiste) printf("Erreur : impossible de charger avion2dp.bmp\n");
    if (!imgLegerDecollAir)   printf("Erreur : impossible de charger avion2d.bmp\n");
    if (!imgLegerParking)     printf("Erreur : impossible de charger avion2.bmp (parking)\n");

    imgAffAir         = lisBMPRGB("avion3.bmp");
    imgAffDesc        = lisBMPRGB("avion3a.bmp");
    imgAffPiste       = lisBMPRGB("avion3ap.bmp");
    imgAffDecollPiste = lisBMPRGB("avion3dp.bmp");
    imgAffDecollAir   = lisBMPRGB("avion3d.bmp");
    imgAffParking     = lisBMPRGB("avion3.bmp");

    if (!imgAffAir)         printf("Erreur : impossible de charger avion3.bmp\n");
    if (!imgAffDesc)        printf("Erreur : impossible de charger avion3a.bmp\n");
    if (!imgAffPiste)       printf("Erreur : impossible de charger avion3ap.bmp\n");
    if (!imgAffDecollPiste) printf("Erreur : impossible de charger avion3dp.bmp\n");
    if (!imgAffDecollAir)   printf("Erreur : impossible de charger avion3d.bmp\n");
    if (!imgAffParking)     printf("Erreur : impossible de charger avion3.bmp (parking)\n");

    // Tailles d'affichage de référence basées sur l'avion de ligne
    if (imgLigneAir)
    {
        airW = imgLigneAir->largeurImage / 4;
        airH = imgLigneAir->hauteurImage / 4;
    }
    else
    {
        airW = 40;
        airH = 40;
    }

    if (imgLigneDesc)
    {
        descW = imgLigneDesc->largeurImage / 4;
        descH = imgLigneDesc->hauteurImage / 4;
    }
    else
    {
        descW = airW;
        descH = airH;
    }

    if (imgLignePiste)
    {
        pisteW = imgLignePiste->largeurImage / 4;
        pisteH = imgLignePiste->hauteurImage / 4;
    }
    else
    {
        pisteW = airW;
        pisteH = airH;
    }

    if (imgLigneParking)
    {
        parkingW = imgLigneParking->largeurImage / 4;
        parkingH = imgLigneParking->hauteurImage / 4;
    }
    else
    {
        parkingW = airW;
        parkingH = airH;
    }

    if (imgLigneDecollPiste)
    {
        decollPisteW = imgLigneDecollPiste->largeurImage / 4;
        decollPisteH = imgLigneDecollPiste->hauteurImage / 4;
    }
    else
    {
        decollPisteW = airW;
        decollPisteH = airH;
    }

    if (imgLigneDecollAir)
    {
        decollAirW = imgLigneDecollAir->largeurImage / 4;
        decollAirH = imgLigneDecollAir->hauteurImage / 4;
    }
    else
    {
        decollAirW = airW;
        decollAirH = airH;
    }
}


void dessineBouton(const Bouton *b, const char *texte)
{
    couleurCourante(200, 200, 200);
    rectangle(b->x1, b->y1, b->x2, b->y2);

    couleurCourante(0, 0, 0);
    epaisseurDeTrait(2);
    ligne(b->x1, b->y1, b->x2, b->y1);
    ligne(b->x2, b->y1, b->x2, b->y2);
    ligne(b->x2, b->y2, b->x1, b->y2);
    ligne(b->x1, b->y2, b->x1, b->y1);
    epaisseurDeTrait(1);

    float largeurTexte = tailleChaine(texte, 20);
    float xTexte = (b->x1 + b->x2)/2 - largeurTexte/2;
    float yTexte = (b->y1 + b->y2)/2 - 10;
    couleurCourante(0, 0, 0);
    afficheChaine(texte, 20, xTexte, yTexte);
}

void dessineAeroport(void)
{
    effaceFenetre(255, 255, 255);

    dessineBouton(&boutonAtterrissage, "Atterrissage");
    dessineBouton(&boutonDecollage, "Decollage");

    // Pistes
    float yBase = 90.f;
    float espacement = 20.f;
    float longueur = 1300.f;

    hauteurPistes[0] = 40.f;
    hauteurPistes[1] = 60.f;
    hauteurPistes[2] = 80.f;

    float yCourant = yBase;
    for (int i = 0; i < NB_PISTES; ++i)
    {
        float h = hauteurPistes[i];
        float y1 = yCourant;
        float y2 = y1 + h;

        centrePistes[i] = (y1 + y2) / 2.0f;

        couleurCourante(100, 100, 100);
        rectangle(600.f, y1, 50.f + longueur, y2);

        couleurCourante(255, 255, 255);
        epaisseurDeTrait(2);
        ligne(600.f, centrePistes[i], 50.f + longueur, centrePistes[i]);
        epaisseurDeTrait(1);

        yCourant = y2 + espacement;
    }

    // Parking
    float xParking1 = 60.f;
    float xParking2 = 200.f;
    float yParking1 = 50.f;
    float yParking2 = 540.f;

    couleurCourante(0, 0, 0);
    rectangle(xParking1, yParking1, xParking2, yParking2);

    int nbPlaces = 8;
    float hPlace = (yParking2 - yParking1) / nbPlaces;
    for (int i = 0; i < nbPlaces; ++i)
    {
        float y1 = yParking1 + i*hPlace + 5;
        float y2 = y1 + hPlace - 10;
        couleurCourante(255, 255, 255);
        rectangle(xParking1 + 10, y1, xParking2 - 10, y2);
    }

    couleurCourante(0, 0, 0);
    afficheChaine("Simulation aeroport", 30, 600, HAUTEUR_FENETRE-70);

    couleurCourante(255, 96, 18);
    afficheChaine("echap pour quitter la simulation", 15, 1100, HAUTEUR_FENETRE-70);
}


void dessineAvionForme(float x, float y)
{
    couleurCourante(255, 255, 0);
    triangle(x-10, y-5, x-10, y+5, x+10, y);
}

void dessineAvions(piste pistes[3],avionsListe* air, parking* prk)
{
    printf("debut3\n");
    avion* avion;
    for(int j=0;j<3;j++){
        avion = pistes[j].avions.premierAvion;
        printf("la1\n");
        afficherEtatAeroport(prk,pistes,air);
        for (int i = 0; i < pistes[j].avions.nbActuelle; i++)
        {
            dessineAvions2(avion);
            avion = avion->avionSuiv;
        }
    }
    avion = air->premierAvion;
    printf("la1\n");
    printf("%d\n",air->nbActuelle);
    for (int i = 0; i < air->nbActuelle; i++)
    {
        avion->emplacement =i;
        dessineAvions2(avion);
        avion = avion->avionSuiv;
    }   
    avion = prk->avions.premierAvion;
    printf("la1\n");
    for (int i = 0; i < prk->avions.nbActuelle; i++)
    {
        dessineAvions2(avion);
        avion = avion->avionSuiv;
    }     
    printf("debut4\n");
}

void dessinesInformations(piste pistes[3], avionsListe* air, parking* prk) {
    char phrase[100];

    // Piste 1
    snprintf(phrase, sizeof(phrase), 
             "Piste 1 : %d", pistes[0].avions.nbActuelle);
    couleurCourante(0, 0, 0);

    afficheChaine(phrase, 20, 10, HAUTEUR_FENETRE - 250);

    // Piste 2
    snprintf(phrase, sizeof(phrase), 
             "Piste 2 : %d", pistes[1].avions.nbActuelle);
    afficheChaine(phrase, 20, 200, HAUTEUR_FENETRE - 250);

    // Piste 3
    snprintf(phrase, sizeof(phrase), 
             "Piste 3 : %d", pistes[2].avions.nbActuelle);
    afficheChaine(phrase, 20, 400, HAUTEUR_FENETRE - 250);

    // Air
    snprintf(phrase, sizeof(phrase), 
             "Air : %d", air->nbActuelle);
    afficheChaine(phrase, 20, 600, HAUTEUR_FENETRE - 250);

    snprintf(phrase, sizeof(phrase), 
             "Parking : %d", prk->avions.nbActuelle);
    afficheChaine(phrase, 20, 800, HAUTEUR_FENETRE - 250);
}


void dessineAvions2(avion* av){
       float longueurPiste = 1300.f;
        DonneesImageRGB *imgAir         = NULL;
        DonneesImageRGB *imgDesc        = NULL;
        DonneesImageRGB *imgPiste       = NULL;
        DonneesImageRGB *imgParking     = NULL;
        DonneesImageRGB *imgDecollPiste = NULL;
        DonneesImageRGB *imgDecollAir   = NULL;

        if (av->categorie == CAT_ligne)
        {
            imgAir         = imgLigneAir;
            imgDesc        = imgLigneDesc;
            imgPiste       = imgLignePiste;
            imgParking     = imgLigneParking;
            imgDecollPiste = imgLigneDecollPiste;
            imgDecollAir   = imgLigneDecollAir;
        }
        else if (av->categorie == leger)
        {
            imgAir         = imgLegerAir;
            imgDesc        = imgLegerDesc;
            imgPiste       = imgLegerPiste;
            imgParking     = imgLegerParking;
            imgDecollPiste = imgLegerDecollPiste;
            imgDecollAir   = imgLegerDecollAir;
        }
        else if (av->categorie == affaire)
        {
            imgAir         = imgAffAir;
            imgDesc        = imgAffDesc;
            imgPiste       = imgAffPiste;
            imgParking     = imgAffParking;
            imgDecollPiste = imgAffDecollPiste;
            imgDecollAir   = imgAffDecollAir;
        }
        else
        {
            // par défaut, on utilise les images de ligne
            imgAir         = imgLigneAir;
            imgDesc        = imgLigneDesc;
            imgPiste       = imgLignePiste;
            imgParking     = imgLigneParking;
            imgDecollPiste = imgLigneDecollPiste;
            imgDecollAir   = imgLigneDecollAir;
        }

        float x = 0.f;
        float y = 0.f;

        switch (av->etat)
        {
        case vol:
        {
            float largeurAffichee = (float) airW;   // taille commune
            float espacement = largeurAffichee + 10.0f;

            x = 50.0f + av->emplacement * espacement;
            y = HAUTEUR_FENETRE - 150.0f;
            break;
        }

        case atterisage:
        {
            float x0 = av->startX;
            float y0 = av->startY;

            float largeurAffichee = (float) descW;
            int p = av->idPiste;
            float x2 = 600.f - largeurAffichee / 2.0f;

            float y2 = centrePistes[p];

            float x1 = (x0 + x2) / 2.0f;
            float y1 = y0 - 50.0f;

            float u = 1.0f - av->t;

            x = u*u*x0 + 2*u*av->t*x1 + av->t*av->t*x2;
            y = u*u*y0 + 2*u*av->t*y1 + av->t*av->t*y2;
            break;
        }

        case attPiste:
        {
            int p = av->idPiste;
            float xStart = 600.f;
            float xEnd   = 50.f + longueurPiste;
            float yPiste = centrePistes[p];

            x = xStart + (xEnd - xStart) * av->t;
            y = yPiste;
            break;
        }

        case sol:
        {
            int idx = av->emplacement;   // rang dans le parking (0..nbPlaces-1)
            int nbPlaces = 8;
            float xParking1 = 60.f;
            float xParking2 = 200.f;
            float yParking1 = 50.f;
            float yParking2 = 540.f;
            float hPlace = (yParking2 - yParking1) / nbPlaces;

            float y1 = yParking1 + idx * hPlace + 5;
            float y2 = y1 + hPlace - 10;

            x = (xParking1 + xParking2) / 2.0f;
            y = (y1 + y2) / 2.0f;
            break;
        }

        case decPiste:
        {
            int p = av->idPiste;
            float xDroite = 50.f + longueurPiste;
            float xGauche = 600.f;
            float yPiste  = centrePistes[p];

            float u = av->t;   // 0 -> 1 : droite -> gauche

            x = xDroite + (xGauche - xDroite) * u;
            y = yPiste;
            break;
        }

        case decollage:
        {
            int p = av->idPiste;
            float xGauche = 600.f;
            float yPiste  = centrePistes[p];

            float largeurAffichee = (float) airW;
            float espacement = largeurAffichee + 10.0f;

            float xVol = 50.0f + av->idPiste * espacement;
            float yVol = HAUTEUR_FENETRE - 150.0f;

            float xStart = xGauche;
            float yStart = yPiste;

            float xEnd = xVol;
            float yEnd = yVol;

            float u = av->t;   // 0 -> 1 pendant la montée

            x = xStart + (xEnd - xStart) * u;
            y = yStart + (yEnd - yStart) * u;
            break;
        }
        case waitPiste:
        {
            int p = av->idPiste;          // numéro de la piste
            float yPisteBas = centrePistes[p] ;

            // Position de base sous la piste
            float xBase = 570.0f;         
            float yBase = yPisteBas; // juste en dessous de la piste

            float espacement = 40.0f;  

            x = xBase- av->emplacement * espacement + 40.0f;
            y = yBase ;
            break;
        }
        }

        if (av->etat == vol && imgAir != NULL)
        {
            afficheImageReduite(
                x - airW / 2.0f,
                y - airH / 2.0f,
                imgAir,
                airW,
                airH);
        }
        else if (av->etat == atterisage && imgDesc != NULL)
        {
            afficheImageReduite(
                x - descW / 2.0f,
                y - descH / 2.0f,
                imgDesc,
                descW,
                descH);
        }
        else if (av->etat == sol && imgParking != NULL)
        {
            afficheImageReduite(
                x - parkingW / 2.0f,
                y - parkingH / 2.0f,
                imgParking,
                parkingW,
                parkingH);
        }
        else if (av->etat == attPiste && imgPiste != NULL)
        {
            afficheImageReduite(
                x - pisteW / 2.0f,
                y - pisteH / 2.0f,
                imgPiste,
                pisteW,
                pisteH);
        }
        else if (av->etat == decPiste)
        {
            if (imgDecollPiste != NULL)
            {
                afficheImageReduite(
                    x - decollPisteW / 2.0f,
                    y - decollPisteH / 2.0f,
                    imgDecollPiste,
                    decollPisteW,
                    decollPisteH);
            }
            else
            {
                dessineAvionForme(x, y);
            }
        }
        else if (av->etat == decollage)
        {
            if (imgDecollAir != NULL)
            {
                afficheImageReduite(
                    x - decollAirW / 2.0f,
                    y - decollAirH / 2.0f,
                    imgDecollAir,
                    decollAirW,
                    decollAirH);
            }
            else
            {
                dessineAvionForme(x, y);
            }
        }
        else if (av->etat == waitPiste)
        {
            if (imgDecollAir != NULL)
            {
                 afficheImageReduite(
                x - decollAirW / 2.0f,
                y - decollAirH / 2.0f,
                imgDecollAir,
                decollAirW,
                decollAirH);
            }
            else
            {
                dessineAvionForme(x, y);
            }
        }
        else
        {
            dessineAvionForme(x, y);
        }

}


bool pointDansBouton(const Bouton *b, int x, int y)
{
    return (x >= b->x1 && x <= b->x2 &&
            y >= b->y1 && y <= b->y2);
}

void clicSouris(piste pistes[3],avionsListe* air, parking* prk)
{
    if (etatBoutonSouris() == GaucheAppuye)
    {
        int x = abscisseSouris();
        int y = ordonneeSouris();

        if (pointDansBouton(&boutonAtterrissage, x, y))
        {
            printf("oui\n");
            realisesAtterrit(pistes,air,prk);
        }
        else if (pointDansBouton(&boutonDecollage, x, y))
        {
            printf("oui\n");
            realisesDecollage(pistes,air,prk,0); //prio vaut 0 car on le dépriorise par rapport aux autrs on depriorises le ligne sur les autres avions
        }
    }
}

void metAJourAnimation(piste pistes[3],avionsListe* air, parking* prk){
    printf("debut1\n");
    avion* ptrAvion;
    for (int i =0;i<3;i++){
        ptrAvion = pistes[i].avions.premierAvion;
        while(ptrAvion!=NULL){
            metAJourAnimation2(pistes,air,prk,ptrAvion);
            ptrAvion = ptrAvion->avionSuiv;
        }
    }
    printf("debut2\n");
}


void metAJourAnimation2(piste pistes[3],avionsListe* air, parking* prk,avion* av)
{
    if (av->etat == atterisage || av->etat == attPiste ||
        av->etat == decPiste   || av->etat == decollage)
    {
        av->t += 0.02f;
        if (av->t >= 1.f)
        {
            av->t = 0.f;
            if (av->etat == atterisage)
            {
                av->etat = attPiste;
            }
            else if (av->etat == attPiste)
            {
                pistes[av->idPiste].avions = delAvionListe(pistes[av->idPiste].avions);                
                av->etat = sol;
                prk->avions = addAvionListe(prk->avions,av);
                pistes[av->idPiste].etat=libre;
                
                ajouteEmplacementPrk(prk->avions,av);
            }
            else if (av->etat == decPiste)
            {
                av->etat = decollage;
            }
            else if (av->etat == decollage)
            {
                av->etat = vol;
                printf("%d\n",pistes[av->idPiste].avions.nbActuelle);
                pistes[av->idPiste].avions = delAvionListe(pistes[av->idPiste].avions);
                *air = addAvionListe(*air,av);
                pistes[av->idPiste].etat=libre;
                ajouteEmplacement(av);
                afficherEtatAeroport(prk,pistes,air);
            }
        }
    }
}
