/**
 * @file UIComboBox.h
 * @brief Définition de la classe UIComboBox, un composant d'interface utilisateur de type liste déroulante.
 *
 * Ce fichier contient la déclaration de la classe UIComboBox, qui permet de créer
 * et de gérer une liste déroulante interactive sur un écran TFT. Il inclut la gestion
 * des éléments, la sélection, le défilement et les interactions utilisateur.
 */

#ifndef UICOMBOBOX_H
#define UICOMBOBOX_H

#include <UITextComponent.h>
#include <U8g2_for_TFT_eSPI.h>
#include "UIComboBoxStyle.h"
#include <vector>
#include <functional>
#include <memory>
 
/**
 * @struct UIComboBoxItem
 * @brief Structure représentant un élément individuel dans la liste déroulante (UIComboBox).
 *
 * Chaque élément est composé d'un texte affichable et d'une valeur entière associée.
 */
struct UIComboBoxItem {
    String text; /**< Le texte affiché pour cet élément. */
    int value;   /**< La valeur entière associée à cet élément. */
};

/**
 * @class UIComboBox
 * @brief Un composant d'interface utilisateur de type liste déroulante (select/combo box).
 *
 * Le UIComboBox permet à l'utilisateur de sélectionner une option parmi une liste prédéfinie.
 * Il peut être étendu pour afficher la liste complète des options et inclut une barre de défilement
 * si le nombre d'éléments dépasse le nombre maximal visible.
 */
class UIComboBox : public UITextComponent {
public:
    /**
     * @brief Type de rappel pour l'événement de repliement de la liste déroulante.
     * @param clearedRect La zone rectangulaire qui a été nettoyée après le repliement.
     */
    using CollapseCallback = std::function<void(const UIRect& clearedRect)>;
    /**
     * @brief Type de rappel pour l'événement de sélection d'un élément dans la liste déroulante.
     * @param selectedIndex L'index de l'élément sélectionné (base zéro).
     * @param selectedValue La valeur entière associée à l'élément sélectionné.
     */
    using SelectCallback = std::function<void(int selectedIndex, int selectedValue)>;

    /**
     * @brief Constructeur de la classe UIComboBox.
     * @param u8f Référence à l'objet U8g2_for_TFT_eSPI utilisé pour le rendu du texte.
     * @param rect La position et les dimensions du composant ComboBox.
     * @param labelText Le texte de l'étiquette affichée au-dessus du ComboBox.
     * @param style Le style visuel du ComboBox, défini par UIComboBoxStyle.
     */
    UIComboBox(U8g2_for_TFT_eSPI& u8f, const UIRect& rect, const String& labelText, const UIComboBoxStyle& style);
    /**
     * @brief Destructeur par défaut de la classe UIComboBox.
     */
    ~UIComboBox() override = default;

    /**
     * @brief Ajoute un nouvel élément à la liste déroulante.
     * @param text Le texte à afficher pour l'élément.
     * @param value La valeur entière associée à l'élément.
     */
    void addItem(const String& text, int value);
    /**
     * @brief Définit l'élément sélectionné par son index.
     * Si l'index est valide, l'élément correspondant est sélectionné et la vue est mise à jour.
     * @param index L'index (base zéro) de l'élément à sélectionner.
     */
    void setSelectedIndex(int index);
    /**
     * @brief Retourne l'index de l'élément actuellement sélectionné.
     * @return L'index de l'élément sélectionné, ou -1 si aucun élément n'est sélectionné.
     */
    int getSelectedIndex() const;
    /**
     * @brief Retourne la valeur associée à l'élément actuellement sélectionné.
     * @return La valeur de l'élément sélectionné, ou -1 si aucun élément n'est sélectionné.
     */
    int getSelectedValue() const;
    /**
     * @brief Retourne le texte de l'élément actuellement sélectionné.
     * @return Le texte de l'élément sélectionné, ou "No selection" si aucun élément n'est sélectionné.
     */
    String getSelectedText() const;

    /**
     * @brief Définit la fonction de rappel à exécuter lorsqu'un élément est sélectionné.
     * @param callback La fonction de rappel de type SelectCallback.
     */
    void setOnSelect(SelectCallback callback);
    /**
     * @brief Définit la fonction de rappel à exécuter lorsque la liste déroulante se replie.
     * @param callback La fonction de rappel de type CollapseCallback.
     */
    void setOnCollapse(CollapseCallback callback);

    /**
     * @brief Gère l'événement de pression tactile sur le composant.
     * Cette méthode est surchargée de UIComponent et gère l'expansion/repliement
     * de la liste, la sélection d'éléments et le défilement.
     * @param tft Référence à l'objet TFT_eSPI pour le dessin.
     * @param tx Coordonnée X de la pression tactile.
     * @param ty Coordonnée Y de la pression tactile.
     */
    void handlePress(TFT_eSPI& tft, int tx, int ty) override;

    /**
     * @brief Vérifie si la liste déroulante est actuellement étendue (dépliée).
     * Cette méthode est surchargée de UIComponent.
     * @return `true` si la liste est dépliée, `false` sinon.
     */
    bool isExpanded() const override;
    /**
     * @brief Replie la liste déroulante si elle est actuellement étendue.
     * Cette méthode est surchargée de UIComponent.
     */
    void collapse() override;

private:
    /**
     * @brief Méthode interne pour dessiner le composant sur l'écran.
     * Cette méthode est surchargée de UIComponent et gère le rendu du ComboBox
     * qu'il soit replié ou étendu, y compris les éléments de la liste et la barre de défilement.
     * @param tft Référence à l'objet TFT_eSPI pour le dessin.
     * @param force Indique si le dessin doit être forcé même si le composant n'est pas marqué comme "sale".
     */
    void drawInternal(TFT_eSPI& tft, bool force) override;
    /**
     * @brief Met à jour la hauteur du composant en fonction de son état (replié ou étendu).
     */
    void updateHeight();

    UIComboBoxStyle _style; /**< Le style visuel actuel du ComboBox. */
    std::vector<UIComboBoxItem> _items; /**< La liste des éléments disponibles dans le ComboBox. */
    int _selectedIndex = -1; /**< L'index de l'élément actuellement sélectionné. -1 si aucun. */
    bool _isExpanded = false; /**< Indique si la liste déroulante est actuellement étendue (dépliée). */

    int _collapsedHeight; /**< La hauteur du ComboBox lorsqu'il est replié. */
    int _maxVisibleItems; /**< Le nombre maximal d'éléments visibles lorsque la liste est étendue. */
    int _scrollOffset = 0; /**< Le décalage de défilement actuel pour les éléments de la liste. */
    int _scrollBarWidth; /**< La largeur de la barre de défilement. */
    uint16_t _scrollBarColor; /**< La couleur de la barre de défilement. */

    SelectCallback _onSelectCallback = nullptr; /**< La fonction de rappel pour l'événement de sélection. */
    CollapseCallback _onCollapseCallback = nullptr; /**< La fonction de rappel pour l'événement de repliement. */

    /**
     * @brief Dessine la barre de défilement si nécessaire.
     * @param tft Référence à l'objet TFT_eSPI pour le dessin.
     */
    void drawScrollBar(TFT_eSPI& tft);
};

#endif // UICOMBOBOX_H