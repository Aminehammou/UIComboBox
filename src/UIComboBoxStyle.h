#ifndef UICOMBOBOXSTYLE_H
#define UICOMBOBOXSTYLE_H

#include "UILabelStyle.h"

struct UIComboBoxStyle {
    // Style pour le label au-dessus du composant
    UILabelStyle labelStyle;

    // Style pour le texte de l'élément sélectionné (dans la boîte fermée)
    UILabelStyle selectedTextStyle;

    // Style pour les éléments dans la liste déroulante
    UILabelStyle itemTextStyle;

    // Couleurs
    uint16_t backgroundColor = TFT_BLACK;
    uint16_t outlineColor = TFT_WHITE;
    uint16_t buttonColor = TFT_DARKGREY;
    uint16_t arrowColor = TFT_WHITE;
    uint16_t highlightColor = TFT_BLUE; // Couleur de l'élément sélectionné dans la liste

    // Dimensions
    int itemHeight = 30;
    int arrowSize = 5;
    int maxVisibleItems = 3; // Nouvelle propriété: nombre maximum d'éléments visibles
    int scrollBarWidth = 10; // Nouvelle propriété: largeur de la barre de défilement
    uint16_t scrollBarColor = TFT_LIGHTGREY; // Nouvelle propriété: couleur de la barre de défilement
};

#endif // UICOMBOBOXSTYLE_H