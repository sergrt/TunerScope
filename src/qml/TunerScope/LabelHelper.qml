import QtQuick

QtObject {
    id: labelHelper

    function shouldDisplayLabel(index) {
        const histogramItemWidthPx = 2;
        const spacingPx = 16;
        const textWidth = "00000".length + 6;
        return (index * histogramItemWidthPx) % (spacingPx + textWidth) === 0;
    }
}
