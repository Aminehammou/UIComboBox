# UIComboBox Library

![UIComboBox Example](https://raw.githubusercontent.com/your-repo/your-image.png) <!-- Placeholder for an image, replace with actual screenshot -->

La bibliothèque `UIComboBox` est un composant d'interface utilisateur de type liste déroulante (combo box) conçu pour les systèmes embarqués, en particulier ceux utilisant des écrans TFT avec les bibliothèques [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) et [U8g2](https://github.com/olikraus/u8g2). Elle offre une gestion complète des éléments, une interaction tactile, une personnalisation avancée du style et la prise en charge du défilement pour les listes longues.

## Fonctionnalités

- **Composant UI autonome:** Facile à intégrer dans vos projets d'interface utilisateur.
- **Gestion des éléments:** Ajoutez des éléments avec un texte affiché et une valeur associée.
- **Sélection d'éléments:** Définissez ou récupérez l'élément sélectionné.
- **Gestion des événements:** Rappels pour la sélection d'un élément (`onSelect`) et la réduction de la liste (`onCollapse`).
- **Interaction tactile:** Gère l'expansion/réduction, la sélection d'éléments et le défilement via des événements tactiles.
- **Personnalisation du style:** Contrôle étendu sur les couleurs, les polices et les dimensions via la structure `UIComboBoxStyle`, incluant la hauteur des éléments, la taille de la flèche, le nombre maximum d'éléments visibles, la largeur et la couleur de la barre de défilement.
- **Affichage du label:** Possibilité d'afficher un label au-dessus du composant.
- **Défilement automatique:** Prend en charge le défilement pour les listes d'éléments dépassant le nombre maximum d'éléments visibles, avec une barre de défilement visuelle.
- **Améliorations récentes:** Correction des problèmes d'affichage du dernier élément et de la bordure inférieure lors du défilement, assurant une expérience utilisateur plus fluide et complète.

## Dépendances

- **[UIComponent](https://github.com/Aminehammou/UIComponent)** : Classe de base pour les éléments d'interface utilisateur.
- **[UILabel](https://github.com/Aminehammou/UILabel)** : Composant pour l'affichage de texte et la gestion du style de texte.
- **[TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)** : Bibliothèque graphique pour les écrans TFT.
- **[U8g2_for_TFT_eSPI](https://github.com/olikraus/u8g2)** : Wrapper U8g2 pour TFT_eSPI, utilisé pour le rendu de texte de haute qualité.
- **[UITypes](https://github.com/Aminehammou/UITypes)** : Contient des structures de données comme `UIRect`, `Theme`, `UIPadding`, `UIRectBox`.

Assurez-vous que ces bibliothèques sont installées et configurées dans votre environnement de développement (PlatformIO ou Arduino IDE).

## Installation

### PlatformIO

Ajoutez les lignes suivantes à votre fichier `platformio.ini` :

```ini
lib_deps =
    UIComboBox
    UIComponent
    UILabel
    TFT_eSPI
    U8g2
    UITypes
```

### Arduino IDE

1. Téléchargez les fichiers ZIP de chaque bibliothèque (UIComboBox, UIComponent, UILabel, TFT_eSPI, U8g2, UITypes) depuis leurs dépôts GitHub respectifs.
2. Dans l'Arduino IDE, allez dans `Croquis > Inclure une bibliothèque > Ajouter la bibliothèque .ZIP...` et sélectionnez chaque fichier ZIP.

## Utilisation Détaillée

### 1. Inclure les en-têtes nécessaires

```cpp
#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>
#include <UIComboBox.h>
#include <UILabel.h>
#include <UIComboBoxStyle.h>
#include <UILabelStyle.h>
#include <UITypes/UIRect.h>
#include <UITypes/Theme.h> // Pour les définitions de couleurs comme TFT_BLACK
```

### 2. Initialisation et Configuration des Styles

Déclarez des instances globales de `TFT_eSPI` et `U8g2_for_TFT_eSPI`, puis configurez les styles pour le ComboBox, son label, le texte sélectionné et les éléments de la liste. Notez les nouvelles propriétés `maxVisibleItems`, `scrollBarWidth` et `scrollBarColor`.

```cpp
TFT_eSPI tft = TFT_eSPI();
U8g2_for_TFT_eSPI u8f;
UIComboBox* myComboBox;

// Définition des styles
UIComboBoxStyle myComboBoxStyle;
UILabelStyle myLabelStyle;
UILabelStyle mySelectedTextStyle;
UILabelStyle myItemTextStyle;

void setup() {
    Serial.begin(115200);
    tft.init();
    // N'oubliez pas d'activer le rétroéclairage de votre afficheur si nécessaire (ex: `tft.setBrightness(255);` ou `digitalWrite(TFT_BL, HIGH);`).
    tft.setRotation(1); // Ou la rotation appropriée pour votre écran
    tft.fillScreen(TFT_BLACK);

    u8f.begin(tft);
    u8f.setFontMode(1); // Fond transparent pour le texte

    // --- Configuration des Styles ---
    myLabelStyle.font = u8g2_font_ncenB08_tr;
    myLabelStyle.textColor = TFT_WHITE;
    myLabelStyle.bgColor = TFT_BLACK;

    mySelectedTextStyle.font = u8g2_font_ncenB10_tr;
    mySelectedTextStyle.textColor = TFT_CYAN;
    mySelectedTextStyle.bgColor = TFT_BLACK;

    myItemTextStyle.font = u8g2_font_ncenR08_tr;
    myItemTextStyle.textColor = TFT_WHITE;
    myItemTextStyle.bgColor = TFT_BLACK;

    myComboBoxStyle.labelStyle = myLabelStyle;
    myComboBoxStyle.selectedTextStyle = mySelectedTextStyle;
    myComboBoxStyle.itemTextStyle = myItemTextStyle;
    myComboBoxStyle.backgroundColor = TFT_DARKGREY;
    myComboBoxStyle.outlineColor = TFT_WHITE;
    myComboBoxStyle.buttonColor = TFT_BLUE;
    myComboBoxStyle.arrowColor = TFT_WHITE;
    myComboBoxStyle.highlightColor = TFT_GREEN;
    myComboBoxStyle.itemHeight = 25;
    myComboBoxStyle.arrowSize = 4;
    myComboBoxStyle.maxVisibleItems = 3; // Nombre maximum d'éléments visibles à la fois
    myComboBoxStyle.scrollBarWidth = 8;  // Largeur de la barre de défilement
    myComboBoxStyle.scrollBarColor = TFT_LIGHTGREY; // Couleur de la barre de défilement

    // --- Création du ComboBox ---
    UIRect comboBoxRect = {50, 50, 150, 30}; // x, y, largeur, hauteur
    myComboBox = new UIComboBox(u8f, comboBoxRect, "Choisissez:", myComboBoxStyle);
}
```

### 3. Ajout d'éléments

```cpp
void setup() {
    // ... initialisation et configuration des styles

    // --- Ajout d'éléments ---
    myComboBox->addItem("Option 1", 10);
    myComboBox->addItem("Option 2", 20);
    myComboBox->addItem("Option 3", 30);
    myComboBox->addItem("Option 4", 40);
    myComboBox->addItem("Option 5", 50);
    myComboBox->addItem("Option 6", 60);
    myComboBox->addItem("Option 7", 70);
    myComboBox->addItem("Option 8", 80);
}
```

### 4. Gestion des événements (Callbacks)

Définissez des fonctions de rappel pour réagir aux interactions de l'utilisateur. La fonction `onListCollapsed` est cruciale pour effacer la zone de la liste après sa réduction.

```cpp
// Fonction de rappel pour gérer la sélection d'un élément
void onItemSelected(int selectedIndex, int selectedValue) {
    Serial.print("Élément sélectionné: ");
    Serial.print(myComboBox->getSelectedText());
    Serial.print(" (Index: ");
    Serial.print(selectedIndex);
    Serial.print(", Valeur: ");
    Serial.print(selectedValue);
    Serial.println(")");
}

// Fonction de rappel pour gérer la réduction de la liste et effacer la zone visible
void onListCollapsed(const UIRect& clearedRect) {
    Serial.println("Liste déroulante repliée. Effacement de la zone visible.");
    // Efface uniquement la zone visible précédemment occupée par la liste déroulante
    tft.fillRect(clearedRect.x, clearedRect.y, clearedRect.w, clearedRect.h, TFT_BLACK); 
    // Vous devrez peut-être redessiner d'autres éléments qui étaient couverts par la liste ici
    // Par exemple, si une image de fond ou d'autres composants UI étaient masqués
}

void setup() {
    // ... initialisation, configuration des styles et ajout d'éléments

    // --- Définition des Callbacks ---
    myComboBox->setOnSelect(onItemSelected);
    myComboBox->setOnCollapse(onListCollapsed);

    // Dessin initial du ComboBox
    myComboBox->draw(tft, true);
}
```

### 5. Dessin et Interaction dans `loop()`

Dans votre boucle `loop()`, vous devez gérer le dessin du composant et les entrées tactiles.

```cpp
// Fonction d'aide pour vérifier si un point est dans un rectangle
bool rectContains(const UIRect& rect, int px, int py) {
    return px >= rect.x && px < (rect.x + rect.w) && py >= rect.y && py < (rect.y + rect.h);
}

static bool touchedLastLoop = false;
static unsigned long touchDebounceTime = 0;
const unsigned long TOUCH_DEBOUNCE_MS = 200;

void loop() {
    uint16_t x, y;
    bool touched = tft.getTouch(&x, &y); // Lit les coordonnées tactiles

    if (touched && !touchedLastLoop && (millis() - touchDebounceTime > TOUCH_DEBOUNCE_MS)) {
        Serial.print("Touch detected at X: ");
        Serial.print(x);
        Serial.print(", Y: ");
        Serial.println(y);

        // Vérifier si le toucher est dans le ComboBox (en-tête ou liste étendue)
        // La méthode handlePress gère maintenant les clics sur l'en-tête, les éléments et la barre de défilement
        if (rectContains(myComboBox->getRect(), x, y) || myComboBox->isExpanded()) {
            myComboBox->handlePress(tft, x, y);
            myComboBox->draw(tft, false); // Redessiner si nécessaire
        }
        touchDebounceTime = millis();
    }
    touchedLastLoop = touched;
}
```



Cette bibliothèque est distribuée sous la licence MIT. Voir le fichier `LICENSE` pour plus de détails.
