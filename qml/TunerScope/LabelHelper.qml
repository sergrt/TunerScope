import QtQuick


QtObject {
    id: labelHelper

    //property int lastDrawnIndex: -10000

    function shouldDisplayLabel(lastDrawnIndex, index, text) {

        //console.log(index + " " + text)
        const spacing = 2; // Ширина одного элемента
        const minGap = 16; // Мин. расстояние в пикселях между надписями
        const textWidth = text.length * 6; // Приблизительно 6px на символ

        if ((index - lastDrawnIndex) * spacing > textWidth + minGap) {
            //lastDrawnIndex = index;
            return [true, index];
        }
        return [false, lastDrawnIndex];
    }
}
