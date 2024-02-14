<?php session_start(); ?>

<?php
/*	inclusion des variables et fonctions */
include_once('variables.php');
include_once('functions.php');

// Validation du formulaire

checkForm($users, $_POST);

?>

<?php if ( ( isset($_SESSION['LOGGED_USER'])) ): ?>
<?php echo 'Succesfull logged'; ?>
<div class="container">

	<img src="https://media2.giphy.com/media/YRuFixSNWFVcXaxpmX/giphy.gif?cid=ecf05e47p6ncusicnhv7uhzrid7q3h3ijgfb8s113je7tswf&ep=v1_gifs_search&rid=giphy.gif&ct=g" width=25% height=25%/>
	<?php foreach(getRecipes($recipes) as $recipe) : ?>
		<article>
			<h3><?php echo $recipe['title']; ?></h3>
			<div><?php echo $recipe['recipe']; ?></div>
			<i><?php echo displayAuthor($recipe['author'], $users); ?></i>
		</article>
		<?php endforeach ?> <br>
<?php else:?>
	<?php echo 'Wrong password'; ?>
	<?php include ('login.php'); ?>
<?php endif; ?>
