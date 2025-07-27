import QtQuick


QtObject {
    id: labelHelper

    //property int lastDrawnIndex: -10000

    function shouldDisplayLabel(index, text) {

        //console.log(index + " " + text)
        const pixelsPerItem = 2; // Ширина одного элемента
        const spacingInPixels = 16; // Мин. расстояние в пикселях между надписями
        const textWidth = "00000".length + 6;//text.length * 6; // Приблизительно 6px на символ

        return (index * pixelsPerItem) % (spacingInPixels + textWidth) === 0;
    }
}
