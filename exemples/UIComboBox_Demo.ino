/**
 * @file UIComboBox_Demo.ino
 * @brief Exemple de démonstration pour la bibliothèque UIComboBox.
 *
 * Cet exemple montre comment initialiser, configurer et interagir avec un composant
 * UIComboBox sur un écran TFT, en utilisant les bibliothèques TFT_eSPI et U8g2_for_TFT_eSPI.
 * Il illustre l'ajout d'éléments, la gestion des événements de sélection et de repliement,
 * ainsi que la gestion des interactions tactiles.
 */

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>
#include <UIComboBox.h>
#include <UIComboBoxStyle.h>
#include <UILabelStyle.h>
#include <UITypes/UIRect.h>
#include <UITypes/Theme.h> // Pour les définitions de couleurs comme TFT_BLACK

// --- Objets Globaux ---
TFT_eSPI tft = TFT_eSPI(); // Instance globale pour l'écran TFT
U8g2_for_TFT_eSPI u8g2;    // Instance globale pour le rendu des polices U8g2
UIComboBox* myComboBox;    // Pointeur vers notre composant UIComboBox

// --- Prototypes de Fonctions de Rappel ---
void onItemSelected(int selectedIndex, int selectedValue);
void onListCollapsed(const UIRect& clearedRect);

// --- Fonctions d'aide pour la gestion tactile ---
bool rectContains(const UIRect& rect, int px, int py) {
    return px >= rect.x && px < (rect.x + rect.w) && py >= rect.y && py < (rect.y + rect.h);
}

// --- Setup --- 
void setup() {
    Serial.begin(115200);
    Serial.println("Démarrage de l'exemple UIComboBox...");

    // Initialisation de l'écran TFT
    tft.begin();
    tft.setRotation(3); // Ajustez la rotation selon votre écran
    tft.fillScreen(TFT_BLACK);

    // Initialisation de U8g2
    u8g2.begin(tft);
    u8g2.setFontMode(1); // Fond transparent pour le texte

    // --- Configuration des Styles pour le ComboBox ---
    UIComboBoxStyle comboBoxStyle;
    comboBoxStyle.labelStyle.font = u8g2_font_profont17_tf;
    comboBoxStyle.labelStyle.textColor = TFT_WHITE;
    comboBoxStyle.selectedTextStyle.font = u8g2_font_profont17_tf;
    comboBoxStyle.selectedTextStyle.textColor = TFT_CYAN;
    comboBoxStyle.itemTextStyle.font = u8g2_font_profont17_tf;
    comboBoxStyle.itemTextStyle.textColor = TFT_WHITE;
    comboBoxStyle.backgroundColor = TFT_DARKGREY;
    comboBoxStyle.outlineColor = TFT_WHITE;
    comboBoxStyle.buttonColor = TFT_BLUE;
    comboBoxStyle.arrowColor = TFT_WHITE;
    comboBoxStyle.highlightColor = TFT_GREEN;
    comboBoxStyle.itemHeight = 25;
    comboBoxStyle.arrowSize = 4;
    comboBoxStyle.maxVisibleItems = 3; // Nombre maximum d'éléments visibles à la fois
    comboBoxStyle.scrollBarWidth = 8;  // Largeur de la barre de défilement
    comboBoxStyle.scrollBarColor = TFT_LIGHTGREY; // Couleur de la barre de défilement

    // --- Création du ComboBox ---
    UIRect comboBoxRect = {10, 50, 200, 30}; // x, y, largeur, hauteur
    myComboBox = new UIComboBox(u8g2, comboBoxRect, "Choisissez une option:", comboBoxStyle);

    // --- Ajout d'éléments au ComboBox ---
    myComboBox->addItem("Option A", 100);
    myComboBox->addItem("Option B", 200);
    myComboBox->addItem("Option C", 300);
    myComboBox->addItem("Option D", 400);
    myComboBox->addItem("Option E", 500);
    myComboBox->addItem("Option F", 600);
    myComboBox->addItem("Option G", 700);

    // --- Définition des Callbacks ---
    myComboBox->setOnSelect(onItemSelected);
    myComboBox->setOnCollapse(onListCollapsed);

    // Dessin initial du ComboBox
    myComboBox->draw(tft, true);
}

// --- Loop --- 
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

    // Optionnel: Mettre à jour d'autres éléments UI ou effectuer d'autres tâches ici
    // myComboBox->draw(tft, false); // Peut être appelé ici si d'autres éléments UI interagissent avec le ComboBox
}

// --- Implémentations des Fonctions de Rappel ---
void onItemSelected(int selectedIndex, int selectedValue) {
    Serial.print("Élément sélectionné: ");
    Serial.print(myComboBox->getSelectedText());
    Serial.print(" (Index: ");
    Serial.print(selectedIndex);
    Serial.print(", Valeur: ");
    Serial.print(selectedValue);
    Serial.println(")");

    // Vous pouvez ajouter ici la logique spécifique à votre application
    // en fonction de l'élément sélectionné.
}

void onListCollapsed(const UIRect& clearedRect) {
    Serial.println("Liste déroulante repliée. Effacement de la zone visible.");
    // Efface uniquement la zone visible précédemment occupée par la liste déroulante
    tft.fillRect(clearedRect.x, clearedRect.y, clearedRect.w, clearedRect.h, TFT_BLACK); 
    // Dans un cas réel, vous devrez peut-être redessiner d'autres éléments qui étaient couverts par la liste ici.
    // Pour cet exemple simple, nous effaçons juste la zone.
}
