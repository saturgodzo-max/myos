// Функция для чтения байта из порта ввода-вывода (из ассемблера)
unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// ИСПРАВЛЕНО: Массив сделан глобальным! Теперь компилятор не ломает стек при старте.
static const unsigned char kbd_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',
    // Все остальные неиспользуемые клавиши заполняем нулями
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

void kernel_main(void) {
    volatile char *video_memory = (volatile char*) 0xB8000;

    // 1. Очищаем экран
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i+1] = 0x07; // Обычный серый текст
    }

    // 2. Выводим зеленую приветственную надпись
    const char *msg = "OS Interactive Mode! Type something on your keyboard:";
    int i = 0;
    while (msg[i] != '\0') {
        video_memory[i * 2] = msg[i];
        video_memory[i * 2 + 1] = 0x0A; // Светло-зеленый цвет
        i++;
    }

    // Позиция курсора для ввода пользователя (строка 3, колонка 0)
    int cursor_pos = 80 * 2 * 2;
    unsigned char last_scancode = 0;

    // 3. Главный бесконечный цикл опроса клавиатуры
    while(1) {
        // Проверяем статус контроллера клавиатуры (порт 0x64)
        if ((inb(0x64) & 1) == 1) {
            unsigned char scancode = inb(0x60); // Читаем скан-код

            // Если клавиша нажата (код < 0x80)
            if (scancode < 0x80) {
                if (scancode != last_scancode) {
                    unsigned char ascii = kbd_map[scancode];

                    if (ascii == '\n') {
                        // Перенос строки
                        cursor_pos = ((cursor_pos / 160) + 1) * 160;
                    } else if (ascii == '\b') {
                        // Обработка Backspace (Стирание)
                        if (cursor_pos > 80 * 2 * 2) { // Не даем стереть приветствие
                            cursor_pos -= 2;
                            video_memory[cursor_pos] = ' ';
                            video_memory[cursor_pos + 1] = 0x07;
                        }
                    } else if (ascii != 0) {
                        // Печатаем обычный символ
                        video_memory[cursor_pos] = ascii;
                        video_memory[cursor_pos + 1] = 0x0F; // Ярко-белый цвет ввода
                        cursor_pos += 2;
                    }
                    last_scancode = scancode;
                }
            }
            // Если клавишу отпустили (код >= 0x80)
            else {
                if ((scancode - 0x80) == last_scancode) {
                    last_scancode = 0;
                }
            }
        }
    }
}





