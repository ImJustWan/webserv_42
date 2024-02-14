<?php
// functions.php

function isValidRecipe(array $recipe) : bool
{
    if (array_key_exists('is_enabled', $recipe)) {
        $isEnabled = $recipe['is_enabled'];
    } else {
        $isEnabled = false;
    }

    return $isEnabled;
}

function displayAuthor(string $authorEmail, array $users) : string
{
    for ($i = 0; $i < count($users); $i++) {
        $author = $users[$i];
        if ($authorEmail === $author['email']) {
            return $author['full_name'] . '(' . $author['age'] . ' ans)';
        }
    }
}

function getRecipes(array $recipes) : array
{
    $validRecipes = [];

    foreach($recipes as $recipe) {
        if (isValidRecipe($recipe)) {
            $validRecipes[] = $recipe;
        }
    }

    return $validRecipes;
}

function checkForm( array $users, array $POST )
{
    if ( !isset($_POST['email']) || !isset($_POST['password']) )
        return ;
    foreach( $users as $user ) {
        if ( $user['email'] == $_POST['email'] && $user['password'] == $_POST['password'] ) {
            $_SESSION['LOGGED_USER'] = $user['full_name'];
        }
    }
}
