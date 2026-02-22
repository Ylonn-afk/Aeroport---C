#ifndef TYPES_H
#define TYPES_H

enum categorieAvion {
    CAT_ligne, // obliger de changer car GfxLib utilise déjà "ligne"
    affaire,
    leger
};


enum etatAvion {
    vol,
    sol,        // parking
    decollage,  // phase de montée après la piste
    atterisage,
    decPiste,   // roulage pour décollage
    attPiste,    // roulage après atterrissage
    waitPiste
};

enum categoriePiste {
    moyenne,
    grande,
    petite
};

enum pisteEtat {
    pisteAtterissage,
    pisteDecollage,
    libre
};

typedef struct avion {
    int id;
    int emplacement;
    int idPiste;
    enum categorieAvion categorie;
    enum etatAvion etat;
    int nbPassager;
    struct avion* avionSuiv;
    //Ajout mélusine
    float t;       // paramètre 0..1 pour les animations
    float startX;  // point de départ pour la descente
    float startY;
} avion;


typedef struct avionsListe {
    int nbActuelle;
    avion* premierAvion;
} avionsListe;

typedef struct piste {
    int id;
    int longueur;
    int idAvion;
    enum categoriePiste categorie;
    enum pisteEtat etat;
    int nbMax;
    avionsListe avions;
} piste;

typedef struct parking {
    int id;
    int nbMax;
    avionsListe avions;
} parking;

// Boutons
typedef struct {
    float x1, y1, x2, y2;
} Bouton;


#endif
