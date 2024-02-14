<!DOCTYPE html>
<html>
	<head>
		<title>Ave Cesar</title>
		<meta charset="UTF-8">
		<link rel="icon" type="image/png" href="/favicon-32x32.png" />
		<link href="/css/cesar.css" rel="stylesheet">
		<link rel="preconnect" href="https://fonts.googleapis.com">
		<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
		<link href="https://fonts.googleapis.com/css2?family=Dancing+Script&display=swap" rel="stylesheet">
	</head>
	<body>
	<div id="container">
		<?php
				function cesar_cipher($letter, $key)
				{
					$charmin = 'abcdefghijklmnopqrstuvwxyz';
					$charmaj = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
					if (($pos = strpos($charmin, $letter)) !== FALSE)
					{
						$newpos = ($pos + $key) % 26;
						return $charmin[$newpos];
					}
					elseif (($pos = strpos($charmaj, $letter)) !== FALSE)
					{
						$newpos = ($pos + $key) % 26;
						return $charmaj[$newpos];
					}
					else
					{
						return $letter;
					}
				}
				$plh = "Votre texte ici";
				if (isset($_POST['cipher']))
				{
					//echo $_SERVER['CONTENT_LENGTH'];
					$texte = $_POST['texte_area'];
					$texte = htmlspecialchars($texte, ENT_QUOTES, 'UTF-8');
					for ($i = 0; $i < strlen($texte); $i++)
					{
						$texte[$i] = cesar_cipher($texte[$i], $_POST['cipher_key']);
					}
				}
				if (isset($_POST['decipher']))
				{
					$texte = $_POST['texte_area'];
					$texte = htmlspecialchars($texte, ENT_QUOTES, 'UTF-8');
					$plh = $texte;
					for ($i = 0; $i < strlen($texte); $i++)
					{
						$texte[$i] = cesar_cipher($texte[$i], -1 * $_POST['cipher_key']);
					}

				}
			?>

		<header id="myHeader">
			<div id="titre principal">
			<h1>Ave Cesar</h1>
			<h2>Chiffreur/Déchiffreur de code Cesar</h2>
		</header>
		<nav id ="menu">
			<a href="/html/page/index2.html">Index</a>
			<a href="/php/cesar_get.php">Ave Cesar (get)</a>
			<a href="/php/cesar_post.php">Ave Cesar (post)</a>
			<a href="/python/quizz.py">The 42 Quizz</a>
			<a href="/php/whoswho.php">Who's who</a>
			<a href="/html/kitty/kitty.html">Kitty</a>
		</nav>
		<section id ="main">
			<p>Veuillez entrer le texte et sa clé</p>
			<form action="cesar_post.php" method="post">
				<div id="ta">
				<textarea type="text" name="texte_area" rows="18" cols="70" placeholder='<?php echo $plh ?>'></textarea>
				</div>
				<div id="ck">
					<select name="cipher_key">
						<option value="0">A</option>
						<option value="1">B</option>
						<option value="2">C</option>
						<option value="3">D</option>
						<option value="4">E</option>
						<option value="5">F</option>
						<option value="6">G</option>
						<option value="7">H</option>
						<option value="8">I</option>
						<option value="9">J</option>
						<option value="10">K</option>
						<option value="11">L</option>
						<option value="12">M</option>
						<option value="13">N</option>
						<option value="14">O</option>
						<option value="15">P</option>
						<option value="16">Q</option>
						<option value="17">R</option>
						<option value="18">S</option>
						<option value="19">T</option>
						<option value="20">U</option>
						<option value="21">V</option>
						<option value="22">W</option>
						<option value="23">X</option>
						<option value="24">Y</option>
						<option value="25">Z</option>
					</select>
				</div>
				<div>
					<input type="submit" name="cipher" value="chiffrer" />
					<input type="submit" name="decipher" value="déchiffrer" />
				</div>
			</form>
			<?php
				if (isset($_POST['cipher']) or isset($_POST['decipher']))
					echo "<div id=\"result\">{$texte}</div>";
			?>
			</section>
			<footer>
				<p>
					<span class="sign"><b>WebServ</b> by pjay and rertzer.</span>
					<span class="devise">Parce que je le code bien.</span>
				</p>
			</footer>
	</div>
	</body>
</html>
