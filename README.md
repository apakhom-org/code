# code

Простенький GPS трекер для отслеживания полета и падения ракеты
Определение координат (u-blox Neo-6M). 
Отсылка СМС (Neoway M590) с координатами после включения.
Ожидание входящего звонка с заданного в скетче номера.
Отсылка СМС на указанный номер после приема(отбоя) звонка. Переход в режим периодической отправки СМС с координатами с заданным в скетче периодом на указанный номер. Ожидание звонка.
После приема (отбоя) звонка отсылка СМС. Останов периодической отсылки СМС. Ожидание звонка.
Далее в цикле указанные выше пункты...

Подключение:
Neo-6M выход Tx к входу Rx Arduino
M590 вход Rx к выходу pin 8 Arduino; выход Tx к входу pin 9 Arduino (библиотека AltSoftSerial)
светодиод pin 13 используется для контроля работы программы (прохождение команд в M590, установление связи GSM, определение спутников
Neo-6M, отправка СМС)

Комментарии:
- отображение числа видимых спутников (на скриншоте "5S" - это пять спутников)
- отсылка СМС при изменении состояния InPort (назначен пин 11, но можно на любой) - на скриншоте "4 С..." - это замкнул на землю, "5 C..." - разомкнул
в самом скетче не забудь поменять номер телефона, скорости обмена с GPS и GSM (у меня 9600)
InPort и InPort2 — это анализируемые входы.
InPort — это вход, по изменению уровня на котором СМС посылается.
