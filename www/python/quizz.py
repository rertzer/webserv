#! /usr/bin/python3

import cgi
import sys
import os

quizz = (("Qui a écrit le <i>Guide Intergalactique</i> ?", "Xavier Niel", "Doug Adams", 2),
        ("Qui n'est pas un personnage du <i>Guide</i> ?", "Marvin", "Norminet", 2),
        ("Les Vogons sont réputés pour leur", "poésie", "hospitalité", 1),
        ("La Terre a été détruite pour laisser place à", "un casino", "une bretelle d'autoroute", 2),
        ("Un voyageur intergalactique se doit d'emporter", "une serviette", "son chat", 1),
        ("Pour communiquer avec les extraterrestres au langage inconnu, on s'enfonce", "un poisson dans l'oreille", "un chat dans la gorge", 1),
        ("Le meilleur restaurant de l'Univers se trouve", "au coins de la rue", "à la fin du monde", 2),
        ("Le président de la Galaxie s'appelle", "Zaphod Beeblebrox", "Elon Musk", 1),
        ("Slartibartfast est connu pour avoir dessiné", "Les côtes de Norvège", "la Joconde", 1))

print("Content-Type: text/html\r\n\r")

print("<!DOCTYPE html>")
print("<html><head><title>The 42 quizz to the Galaxy</title><meta charset=\"UTF-8\"><link href=\"/css/cesar.css\" rel=\"stylesheet\"></head>")
print("<body><h1>The 42 quizz to the Galaxy</h1>")

form = cgi.FieldStorage()
q_id = form.getvalue('q_id')
q_answer = form.getvalue('q_answer')

if not q_id:
    q_id = 0

q_id = int(q_id)
if q_id > 8:
    print("Le quizz est fini. Merci d'avoir participé")
else:
    if not q_answer:
        print("<p>Question numero ", q_id, "</p>")
        print("<p>{}</p>".format(quizz[q_id][0]))
        print("<form action=\"quizz.py\" method=\"get\">")
        print("<input type=\"hidden\" name=\"q_id\" value=\"{}\" />".format(q_id))
        print("<input type=\"submit\" name=\"q_answer\" value=\"{}\" />".format(quizz[q_id][1]))
        print("<input type=\"submit\" name=\"q_answer\" value=\"{}\" />".format(quizz[q_id][2]))
        print("</form>")
    else:
        good = quizz[q_id][3]
        if q_answer == quizz[q_id][good]:
            print("<p>Bravo !", q_answer, "est la bonne réponse.</p>")
        else:
            print("<p>Raté !", q_answer, "est la mauvaise réponse.</p>")
        print("<form action=\"quizz.py\" method=\"get\">")
        print("<input type=\"hidden\" name=\"q_id\" value=\"{}\" />".format(q_id + 1))
        print("<input type=\"submit\" value=\"question suivante\" />")
        print("</form>")
print("</body>")
