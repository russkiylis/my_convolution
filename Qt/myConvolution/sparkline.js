// Функиця для рисования спарклайна по заданной ширине, высоте и массиву чисел
function drawSparkline(ctx, width, height, data, color = "black", lineWidth = 1) {
    ctx.clearRect(0, 0, width, height);     // Очищаем прямоугольник

    if (!data || data === 0) return;

    const min = Math.min.apply(null, data);     // Находим минимальное значение
    const max = Math.max.apply(null, data);     // Находим максимальное значение
    const range = max - min || 1;   // Разброс

    const stepX = width / (data.length - 1);

    // Функция для нахождения координаты по Y
    function getY(value) {
        const normalizedValue = (value - min) / range; // Нормируем значения чтобы были от 0 до 1
        return height - (normalizedValue * (height - lineWidth * 2)) - lineWidth;   // Высчитываем координату
    }

    ctx.beginPath();
    ctx.moveTo(0, getY(data[0]));

    for (let i = 1; i < data.length - 1; i++) {
        const x = i * stepX;
        const y = getY(data[i]);
        ctx.lineTo(x, y);
    }
    ctx.lineTo(width, getY(data[data.length - 1]));
    ctx.strokeStyle = color;
    ctx.lineWidth = lineWidth;
    ctx.stroke();
}