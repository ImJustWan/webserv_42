<!DOCTYPE html>
<html>
	<head>
		<title>Who's Who</title>
		<meta charset="UTF-8">
		<link rel="icon" type="image/png" href="/favicon-32x32.png" />
		<link href="/css/who.css" rel="stylesheet">
		<link rel="preconnect" href="https://fonts.googleapis.com">
		<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
		<link href="https://fonts.googleapis.com/css2?family=Dancing+Script&display=swap" rel="stylesheet">
	</head>
	<body>
		<?php
			$whois = array(
				"Arthur Dent" => "Anglais moyen voyageant en peignoir &agrave travers la Galaxie.",
				"Ford Prefect" => "Voiture britannique (produite entre 1938 et 1961). Accessoirement un extraterrestre travaillant pour le <i>Guide InterGalactique.</i>",
				"Zaphod Beeblebrox" => "Pr&eacutesident bic&eacutephale de la Galaxie.",
				"Marvin the Paranoid Android" => "Trop intelligent pour &ecirctre heureux.",
				"Trillian" => "Brillante astrophysicienne. Arthur a des vues sur elles, mais elle sort avec Zaphod.",
				"Slartibartfast" => "Magrath&eacuteen. Se passionne pour le dessin des lignes de c&ocircte.",
				"Agrajag" => "Running gag. Ses r&eacuteincarnations successives sont syst&eacutematiquement tu&eacutees par Arthur Dent.",
				"Deep Thought" => "Ordinateur. Il trouva la r&eacuteponse 42. Ce qui lui prit sept millions et demi d'ann&eacutees.",
				"Dish of the Day" => "Une vache. Elle adhore &ecirctre mang&eacutee.",
				"Elvis Presley" => "Eh non, il n'est pas mort, mais continue sa carri&egravere dans la Galaxie.",
				"Grunthos the Flatulent" => "Po&egravete vogon."
			);
		?>
		<div id="container">

			<header id="myHeader">
				<div id="titre principal">
				<h1>Who's Who</h1>
				<h2>The Ultimate Galaxie's Who's Who</h2>
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
				<?php
					if (isset($_SERVER['PATH_INFO']))
					{
						$texte = $_SERVER['PATH_INFO'];
						$texte = htmlspecialchars($texte, ENT_QUOTES, 'UTF-8');
						$texte = str_replace("_", " ", $texte);
						echo '<h1>', $texte, '</h1><p>', $whois[$texte], '</p>';
					}
				?>
			</section>
			<nav id="who">
				<a href="/php/whoswho.php/Arthur_Dent">Arthur Dent</a>
				<a href="/php/whoswho.php/Ford_Prefect">Ford Prefect</a>
				<a href="/php/whoswho.php/Zaphod_Beeblebrox">Zaphod Beeblebrox</a>
				<a href="/php/whoswho.php/Marvin_the_Paranoid_Android">Marvin the Paranoid Android</a>
				<a href="/php/whoswho.php/Trillian">Trillian</a>
				<a href="/php/whoswho.php/Slartibartfast">Slartibartfast</a>
				<a href="/php/whoswho.php/Agrajag">Agrajag</a>
				<a href="/php/whoswho.php/Deep_Thought">Deep Thought</a>
				<a href="/php/whoswho.php/Dish_of_the_Day">Dish of the Day</a>
				<a href="/php/whoswho.php/Elvis_Presley">Elvis Presley</a>
				<a href="/php/whoswho.php/Grunthos_the_Flatulent">Grunthos the Flatulent</a>
			</nav>
			<footer>
				<p>
					<span class="sign"><b>WebServ</b> by pjay and rertzer</span>
					<span class="devise">Parce que je le code bien.</span>
				</p>
			</footer>
		</div>
	</body>
</html>
