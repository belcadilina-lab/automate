flagging = -o
main : main.c
	gcc $(flagging) main dependencies/manipulation.c dependencies/optimisation.c dependencies/utilities.c dependencies/lectureEtAffichage.c dependencies/verification.c dependencies/operations.c main.c   