#include "UIComboBox.h"

// Constantes pour le style et la clarté du code
namespace {
    constexpr int TEXT_PADDING_X = 10;
}

UIComboBox::UIComboBox(U8g2_for_TFT_eSPI& u8f, const UIRect& rect, const String& labelText, const UIComboBoxStyle& style)
    : UITextComponent(u8f, rect, labelText), // Call base constructor
      _style(style), _collapsedHeight(rect.h),
      _maxVisibleItems(style.maxVisibleItems),
      _scrollBarWidth(style.scrollBarWidth),
      _scrollBarColor(style.scrollBarColor)
{
    // No need to create a UILabel anymore
}

void UIComboBox::addItem(const String& text, int value) {
    _items.push_back({text, value});
    if (_selectedIndex == -1 && !_items.empty()) {
        _selectedIndex = 0;
    }
    setDirty(true); // La liste a changé, il faut redessiner.
}

void UIComboBox::setSelectedIndex(int index) {
    if (index >= 0 && (size_t)index < _items.size()) {
        if (_selectedIndex != index) {
            _selectedIndex = index;
            // Ajuster le scrollOffset pour que l'élément sélectionné soit visible
            if (_selectedIndex < _scrollOffset) {
                _scrollOffset = _selectedIndex;
            } else if (_selectedIndex >= _scrollOffset + _maxVisibleItems) {
                _scrollOffset = _selectedIndex - _maxVisibleItems + 1;
            }
            if (_onSelectCallback) {
                _onSelectCallback(_selectedIndex, getSelectedValue());
            }
            setDirty(true);
        }
    }
}

int UIComboBox::getSelectedIndex() const {
    return _selectedIndex;
}

int UIComboBox::getSelectedValue() const {
    if (_selectedIndex >= 0 && (size_t)_selectedIndex < _items.size()) {
        return _items[_selectedIndex].value;
    }
    return -1;
}

String UIComboBox::getSelectedText() const {
    if (_selectedIndex >= 0 && (size_t)_selectedIndex < _items.size()) {
        return _items[_selectedIndex].text;
    }
    return F("No selection"); // Utiliser F() pour stocker la chaîne en Flash
}

void UIComboBox::setOnSelect(SelectCallback callback) {
    _onSelectCallback = callback;
}

void UIComboBox::setOnCollapse(CollapseCallback callback) {
    _onCollapseCallback = callback;
}

void UIComboBox::updateHeight() {
    if (_isExpanded) {
        // La hauteur étendue est basée sur le nombre max d'éléments visibles
        int visibleItemsCount = std::min((int)_items.size(), _maxVisibleItems);
        rect.h = _collapsedHeight + visibleItemsCount * _style.itemHeight;
    } else {
        rect.h = _collapsedHeight;
    }
}

void UIComboBox::drawInternal(TFT_eSPI& tft, bool force) {
    // Draw the label manually
    if (!_text.isEmpty()) {
        _u8f.setFontMode(1);
        _u8f.setFont(_style.labelStyle.font);
        _u8f.setForegroundColor(_style.labelStyle.textColor);
        // Position the label above the combo box
        int fontHeight = _u8f.getFontAscent() - _u8f.getFontDescent();
        int labelY = rect.y - fontHeight + 12;
        _u8f.setCursor(rect.x, labelY);
        _u8f.print(_text);
    }

    // Couleurs basées sur l'état et le style
    uint16_t mainBgColor = enabled ? _style.backgroundColor : TFT_DARKGREY;
    uint16_t buttonBgColor = enabled ? _style.buttonColor : TFT_DARKGREY;
    uint16_t outlineColor = enabled ? _style.outlineColor : TFT_DARKGREY;
    uint16_t textColor = enabled ? _style.selectedTextStyle.textColor : TFT_LIGHTGREY;
    uint16_t arrowColor = enabled ? _style.arrowColor : TFT_LIGHTGREY;

    // --- Dimensions pour le style TComboBox ---
    int buttonWidth = _collapsedHeight; // Un bouton carré
    int textAreaWidth = rect.w - buttonWidth;
    int buttonX = rect.x + textAreaWidth;

    // --- Dessiner la boîte fermée ---
    tft.fillRect(rect.x, rect.y, textAreaWidth, _collapsedHeight, mainBgColor);
    tft.fillRect(buttonX, rect.y, buttonWidth, _collapsedHeight, buttonBgColor);
    tft.drawRect(rect.x, rect.y, rect.w, _collapsedHeight, outlineColor); // Bordure extérieure
    tft.drawFastVLine(buttonX, rect.y + 1, _collapsedHeight - 2, outlineColor); // Séparateur

    // --- Dessiner le texte sélectionné avec U8g2 ---
    _u8f.setFontMode(1);
    _u8f.setFont(_style.selectedTextStyle.font);
    _u8f.setForegroundColor(textColor);
    int16_t textH = _u8f.getFontAscent() - _u8f.getFontDescent();
    int textY = rect.y + (_collapsedHeight + textH) / 2;
    _u8f.setCursor(rect.x + TEXT_PADDING_X, textY);
    _u8f.print(getSelectedText());

    // --- Dessiner la flèche dans le bouton ---
    int arrowX = buttonX + (buttonWidth - _style.arrowSize * 2) / 2;
    int arrowY = rect.y + (_collapsedHeight - _style.arrowSize) / 2;
    if (_isExpanded) {
        tft.fillTriangle(arrowX, arrowY + _style.arrowSize, arrowX + _style.arrowSize * 2, arrowY + _style.arrowSize, arrowX + _style.arrowSize, arrowY, arrowColor); // Flèche vers le haut
    } else {
        tft.fillTriangle(arrowX, arrowY, arrowX + _style.arrowSize * 2, arrowY, arrowX + _style.arrowSize, arrowY + _style.arrowSize, arrowColor); // Flèche vers le bas
    }

    // --- Dessiner la liste si dépliée ---
    if (_isExpanded) {
        int listTopY = rect.y + _collapsedHeight;
        uint16_t listBgColor = _style.backgroundColor;

        int visibleItemsCount = std::min((int)_items.size(), _maxVisibleItems);
        int listHeight = visibleItemsCount * _style.itemHeight;

        // Largeur de la zone de texte des éléments (sans la barre de défilement)
        int itemTextWidth = rect.w;
        if (_items.size() > _maxVisibleItems) { // Si une barre de défilement est nécessaire
            itemTextWidth -= _scrollBarWidth;
        }

        tft.fillRect(rect.x, listTopY, rect.w, listHeight, listBgColor);

        _u8f.setFont(_style.itemTextStyle.font);

        int startIndex = _scrollOffset;
        int endIndex = std::min((int)_items.size(), _scrollOffset + _maxVisibleItems);

        for (int i = startIndex; i < endIndex; ++i) {
            int itemIndex = i; // itemIndex is now just i

            uint16_t itemBgColor = (itemIndex == _selectedIndex) ? _style.highlightColor : listBgColor;
            uint16_t itemTextColor = (itemIndex == _selectedIndex) ? _style.itemTextStyle.bgColor : _style.itemTextStyle.textColor;

            int currentItemY = listTopY + (i - startIndex) * _style.itemHeight;

            tft.fillRect(rect.x + 1, currentItemY + 1, itemTextWidth - 2, _style.itemHeight - 1, itemBgColor);
            
            _u8f.setForegroundColor(itemTextColor);
            int16_t itemTextH = _u8f.getFontAscent() - _u8f.getFontDescent();
            int itemTextY = currentItemY + (_style.itemHeight + itemTextH) / 2;
            _u8f.setCursor(rect.x + TEXT_PADDING_X, itemTextY);
            _u8f.print(_items[itemIndex].text);
        }

        // Dessiner la barre de défilement si nécessaire
        if (_items.size() > _maxVisibleItems) {
            drawScrollBar(tft);
        }
        tft.drawRect(rect.x, listTopY, rect.w, listHeight, outlineColor); // Dessiner la bordure en dernier
    }
}

void UIComboBox::drawScrollBar(TFT_eSPI& tft) {
    int listTopY = rect.y + _collapsedHeight;
    int visibleListHeight = std::min((int)_items.size(), _maxVisibleItems) * _style.itemHeight;

    int scrollBarX = rect.x + rect.w - _scrollBarWidth;

    // Fond de la barre de défilement
    tft.fillRect(scrollBarX, listTopY, _scrollBarWidth, visibleListHeight, _style.backgroundColor);
    tft.drawRect(scrollBarX, listTopY, _scrollBarWidth, visibleListHeight, _style.outlineColor);

    // Calcul de la taille et de la position du pouce (thumb) de la barre de défilement
    float itemsRatio = (float)_maxVisibleItems / _items.size();
    int thumbHeight = (int)(visibleListHeight * itemsRatio);
    if (thumbHeight < 10) thumbHeight = 10; // Taille minimale du pouce

    float scrollRatio;
    int maxScrollOffset = _items.size() - _maxVisibleItems;
    if (maxScrollOffset <= 0) { // No scrolling needed or only one "page" of items
        scrollRatio = 0.0f;
    } else {
        scrollRatio = (float)_scrollOffset / (float)maxScrollOffset;
        // Clamp scrollRatio to ensure it doesn't exceed 1.0 due to precision or rounding
        if (scrollRatio > 1.0f) scrollRatio = 1.0f;
    }
    int thumbY;
    if (maxScrollOffset <= 0) {
        thumbY = listTopY; // No scrolling, thumb at top
    } else {
        // Calculate thumb position based on scrollRatio
        thumbY = listTopY + (int)((visibleListHeight - thumbHeight) * scrollRatio);
        
        // Ensure thumb reaches the very bottom when scrolled to max
        if (_scrollOffset == maxScrollOffset) {
            thumbY = listTopY + visibleListHeight - thumbHeight;
        }
    }

    tft.fillRect(scrollBarX + 1, thumbY + 1, _scrollBarWidth - 2, thumbHeight - 2, _scrollBarColor);
}

void UIComboBox::handlePress(TFT_eSPI& tft, int tx, int ty) {
	if (!enabled) return;

	// Clic sur l'en-tête pour déplier/replier
	bool inHeader = (tx >= rect.x && tx <= rect.x + rect.w && ty >= rect.y && ty <= rect.y + _collapsedHeight);
	if (inHeader) {
		if (_isExpanded) {
			collapse();
		} else {
			_isExpanded = true;
            // Réinitialiser le scrollOffset lors de l'expansion
            _scrollOffset = 0;
			updateHeight();
			setDirty(true);
		}
		return;
	}

	// Si la liste est dépliée, vérifier un clic sur un item ou la barre de défilement
	if (_isExpanded) {
		int listTopY = rect.y + _collapsedHeight;
        int visibleListHeight = std::min((int)_items.size(), _maxVisibleItems) * _style.itemHeight;
        int scrollBarX = rect.x + rect.w - _scrollBarWidth;

        // Clic sur la barre de défilement
        if (_items.size() > _maxVisibleItems && tx >= scrollBarX && tx <= rect.x + rect.w && ty >= listTopY && ty <= listTopY + visibleListHeight) {
            // Calculer la nouvelle position du pouce en fonction du clic
            float clickRatio = (float)(ty - listTopY) / visibleListHeight;
            int maxScrollOffset = _items.size() - _maxVisibleItems;
            
            // Ensure maxScrollOffset is not negative
            if (maxScrollOffset < 0) maxScrollOffset = 0;

            _scrollOffset = (int)(maxScrollOffset * clickRatio);

            // S'assurer que _scrollOffset reste dans les limites
            _scrollOffset = std::max(0, std::min(maxScrollOffset, _scrollOffset));
            // Si le clic est dans la dernière moitié de la zone de défilement, s'assurer d'atteindre le maxScrollOffset
            if (ty >= listTopY + visibleListHeight - _style.itemHeight / 2) {
                _scrollOffset = maxScrollOffset;
            }

            // S'assurer que _scrollOffset reste dans les limites
            if (_scrollOffset < 0) _scrollOffset = 0;
            if (_scrollOffset > maxScrollOffset) _scrollOffset = maxScrollOffset;
            setDirty(true);
            return;
        }

		for (int i = 0; i < _maxVisibleItems; ++i) {
            int itemIndex = _scrollOffset + i;
            if (itemIndex >= (int)_items.size()) break; // Sécurité

			int itemY = listTopY + i * _style.itemHeight;
			if (tx >= rect.x && tx <= rect.x + rect.w - (_items.size() > _maxVisibleItems ? _scrollBarWidth : 0) && ty >= itemY && ty <= itemY + _style.itemHeight) {
				setSelectedIndex(itemIndex); // Sélectionne l'item visible
				collapse();          // Et replie la liste
				return;
			}
		}
		// Si on arrive ici, le clic était dans la zone du composant mais pas sur un item ou la barre de défilement.
		// On considère que c'est un clic "à côté" qui doit fermer la liste.
		collapse();
	}
}

bool UIComboBox::isExpanded() const {
    return _isExpanded;
}

void UIComboBox::collapse() {
    if (_isExpanded) {
        // Avant de replier, on mémorise la zone qu'occupait la liste.
        UIRect dropdownListRect = {
            rect.x,
            rect.y + _collapsedHeight,
            rect.w,
            (int)(std::min((int)_items.size(), _maxVisibleItems) * _style.itemHeight)
        };

        _isExpanded = false;
        updateHeight();
        setDirty(true);
        if (_onCollapseCallback) {
            _onCollapseCallback(dropdownListRect); // On passe la région à nettoyer
        }
    }
}