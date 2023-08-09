<DOCTYPE html>
<html>
	<head>
		<title>Ave Cesar</title>
	</head>
	<body>
		<h1>Ave Cesar</h1>
		<h2>Chiffreur/Déchiffreur de code Cesar</h2>
		<p>Veuillez entrer le texte et sa clé</p>
		<form action="cesar.php" method="post">
			<div>
				<textarea type="text" name="texte_area" rows="8" cols="70" maxlength=10000 value="">
				</textarea>
				<select name="cypher_key">
					<option value="1">A</option>
					<option value="2">B</option>
					<option value="3">C</option>
					<option value="4">D</option>
					<option value="5">E</option>
					<option value="6">F</option>
					<option value="7">G</option>
					<option value="8">H</option>
					<option value="9">I</option>
					<option value="10">J</option>
					<option value="11">K</option>
					<option value="12">L</option>
					<option value="13">M</option>
					<option value="14">N</option>
					<option value="15">O</option>
					<option value="16">P</option>
					<option value="17">Q</option>
					<option value="18">R</option>
					<option value="19">S</option>
					<option value="20">T</option>
					<option value="21">U</option>
					<option value="22">V</option>
					<option value="23">W</option>
					<option value="24">X</option>
					<option value="25">Y</option>
					<option value="26">Z</option>
				</select>
			<div>
			</div>
				<input type="submit" name="cipher" value="chiffrer" />
				<input type="submit" name="decipher" value="déchiffrer" />
			</div>
		</form>
		<?php
			if (isset($_POST['cipher']))
			{
				$texte = $_POST['texte_area'];
				echo "$texte";
			}
		?>
	</body>
</html>
