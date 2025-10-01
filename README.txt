 - Za Kompajliranje projekta pratiti sledece postupke (komande):
	1. otvoriti Terminal/PowerShell/cmd u root direktorijumu projekta
	2. cd src
	3. gcc -o app main.c agrpod.c ofseti.c polaganje_ispita.c student.c test.c
 - Za pokretanje resenja:
	1. komnda:
		app
 - Testni podaci:
	1. Za datoteku koja cuva informacije o studentu(putanja iz root direktorijuma projekta):
		/podaci/student/studenti.bin
	2. Za datoteku koja cuva informacije o polaganjima ispita(putanja iz root direktorijuma projekta):
		/podaci/polaganje_ispita/polaganja.bin
	3. Za datoteku koja cuva agregirane podatke (putanja iz root direktorijuma projekta):
		/podaci/index_sek/agrpod.bin
		