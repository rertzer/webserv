#! /usr/bin/python3

import cgi
import sys
import os

quizz = (("Qui a écrit le <i>Guide Intergalactique</i> ?", "Xavier Niel", "Doug Adams", 2),
        ("Qui n'est pas un personnage du <i>Guide</i> ?", "Marvin", "Norminet", 2),
        ("Les Vogons sont réputés pour leur", "poésie", "hospitalité", 1),
        ("La Terre a été détruite pour laisser place à", "un casino", "une bretelle d'autoroute", 2),
        ("Le mieux pour voyager dans la Galaxie est d'emporter", "une serviette", "son chat", 1),
        ("Pour communiquer avec des extraterrestres au langage inconnu, on se glisse", "un poisson dans l'oreille", "un chat dans la gorge", 1),
        ("Le meilleur restaurant de l'Univers se trouve", "à la fin des temps", "au coins de la rue", 1))

print("Content-type: text/html\r")

print("<!DOCTYPE html")
print("<html><head><title>The 42 quizz to the Galaxy</title><meta charset=\"UTF-8\"><link href=\"cesar.css\" rel=\"stylesheet\"></head>")
print("<body><h1>The 42 quizz to the Galaxy</h1>")

form = cgi.FieldStorage()
q_number = form.getvalue('q_number')
q_id = form.getvalue('q_id')
q_answer = form.getvalue('q_answer')

if not q_number:
    q_number = 1
    q_id = 1

if not q_answer:
    print("<p>Question numero ", q_number, "</p>")
    print("<p class="question">", quizz[q_id][0], "</p>")
    print("<form class="choice">", quiss[q_id[1])
    print("<a href= class="choice">", quiss[q_id[2], "</p>")


