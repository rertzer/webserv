#! /usr/bin/python3

import cgi
import sys
import os
from http import cookies

quizz = (("Qui a écrit le <i>Guide Intergalactique</i> ?", "Xavier Niel", "Doug Adams", 2),
        ("Qui n'est pas un personnage du <i>Guide</i> ?", "Marvin", "Norminet", 2),
        ("Les Vogons sont réputés pour leur", "poésie", "hospitalité", 1),
        ("La Terre a été détruite pour laisser place à", "un casino", "une bretelle d'autoroute", 2),
        ("Un voyageur intergalactique se doit d'emporter", "une serviette", "son chat", 1),
        ("Pour communiquer avec les extraterrestres au langage inconnu, on s'enfonce", "un poisson dans l'oreille", "un chat dans la gorge", 1),
        ("Le meilleur restaurant de l'Univers se trouve", "au coins de la rue", "à la fin du monde", 2),
        ("Le président de la Galaxie s'appelle", "Zaphod Beeblebrox", "Elon Musk", 1),
        ("Slartibartfast est connu pour avoir dessiné", "Les côtes de Norvège", "la Joconde", 1))

#--------------------------------------------------------------
cookie = cookies.BaseCookie()
if 'HTTP_COOKIE' in os.environ:
    print("cookie found", file=sys.stderr)
    cookie.load(os.environ["HTTP_COOKIE"])
    for a, morsel in cookie.items():
        print(a, ':', morsel.value, file=sys.stderr)
#---------------------------------------------------------------

#if not "name" in cookie:
#    cookie["name"] = "anonymous"
if not "good" in cookie:
    cookie["good"] = "0"
if not "total" in cookie:
    cookie["total"] = "0"

#print("name is", cookie["name"].key, cookie["name"].value, file=sys.stderr)
print("good is", cookie["good"].key, cookie["good"].value, file=sys.stderr)
print("total is", cookie["total"].key, cookie["total"].value, file=sys.stderr)



content = "<!DOCTYPE html>"
content += "<html><head><title>The 42 quizz to the Galaxy</title><meta charset=\"UTF-8\"><link href=\"/css/cesar.css\" rel=\"stylesheet\"></head>"
content += "<body><h1>The 42 quizz to the Galaxy</h1>"
form = cgi.FieldStorage()
c_name = form.getvalue('c_name')
q_id = form.getvalue('q_id')
q_answer = form.getvalue('q_answer')


if not q_id:
    q_id = 0
    cookie["good"] = "0"
    cookie["total"] = "0"

q_id = int(q_id)

if not "name" in cookie and c_name:
        cookie["name"] = c_name

if not "name" in cookie:
    content += "<p>Salut jeune voyageur !</p><p>Quel est ton nom ?</p>"
    content += "<form action=\"quizz.py\" method=\"get\">"
    content += "<input type=\"text\" name=\"c_name\" />"
    content += "<input type=\"submit\" name=\"nicky\" value=\"send\" />"
    content += "</form>"
elif q_id > 8:
   content += "Le quizz est fini. Merci {} d'avoir participé".format(cookie["name"].value)
   content += "<p>Tu as {} bonne(s) reponse(s) sur {}</p>".format(cookie["good"].value, cookie["total"].value)

else:
    if q_answer:
        good = quizz[q_id][3]
        tmp = int(cookie["total"].value) + 1
        cookie["total"] = tmp
        if q_answer == quizz[q_id][good]:
            content += "<p>Bravo " + cookie["name"].value + " ! " + q_answer + " est la bonne réponse.</p>"
            tmp = int(cookie["good"].value) + 1
            cookie["good"] = tmp
        else:
            content += "<p>Raté " + cookie["name"].value + " ! " + q_answer + " est la mauvaise réponse.</p>"
        q_id += 1

    content +="<p>Question numero " + "{}".format(q_id) + "</p>"
    content += "<p>" + cookie["name"].value + ", " + quizz[q_id][0] + "</p>"
    content += "<form action=\"quizz.py\" method=\"get\">"
    content += "<input type=\"hidden\" name=\"q_id\" value=\"{}\" />".format(q_id)
    content += "<input type=\"submit\" name=\"q_answer\" value=\"" + quizz[q_id][1] + "\" />"
    content +=  "<input type=\"submit\" name=\"q_answer\" value=\"" + quizz[q_id][2] + "\" />"
    content +=  "</form>"
    content += "</body>"

print(cookie, end="\r\n")
print("Content-Type: text/html", end="\r\n")
print("", end="\r\n")
print(content)
