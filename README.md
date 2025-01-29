# hangman-sk2
Gracz łączy się do serwera i wysyła swój nick (jeśli nick jest już zajęty,
serwer prosi o podanie innego nicku).
Gracz po wybraniu nicku trafia do lobby, w którym widzi bieżącą listę pokoi
i ilość graczy która jest w każdym pokoju.

Z lobby gracz może wejść do istniejącego pokoju lub założyć nowy. Na
serwerze nie ma ograniczenia liczby pokoi. Gracz może w każdej chwili
wrócić z pokoju do lobby.

Jeśli w pokoju trwa gra, gracz dołącza jako zgadujący. Jeśli nie, gracz
widzi listę graczy którzy są w pokoju i czeka na rozpoczęcie gry.

Grę może rozpocząć gracz który najdłużej czeka w pokoju, pod warunkiem że w
pokoju jest maxymalnie 4 graczy, a minimalnie 2.
Gracze otrzymują losowe hasło do odgadnięcia(wszyscy to samo) i rozpoczynają
grę w wisielca, zgadujący widzą ilość liter do odgadnięcia, zgadują kolejno
litery, za złą literę otrzymują kolejny etap w wisielcu. Gracze nie widzą
jakie litery odgadli inni gracze, jednak widzą czy posiadają poprawną
literę na danej pozycji, natomiast gracze widzą wisielce innych graczy. Gra
zakończy się gdy zostanie 1 aktywny gracz, który nie odgadł hasła. Tworzone
jest podium(tylko osoby co odgadły). Po skończonej grze przenosi graczy do
lobby, gdzie mogą zacząć kolejną grę.