<?php
    if ($_SERVER['SERVER_PORT'] == 80) { // nginx
        // encode $_SERVER as json
        echo json_encode($_SERVER);
    } else { // serveurtoile
        $content = file_get_contents('http://localhost:8081/env_diff.php');
        $json = json_decode($content, true);
        // compare $_SERVER with json
        // values different = RED
        // values that are missing = BLUE
        // values that are the same = GREEN
        echo "<table><thead><tr><th style='text-align: center;'>Variable</th><th style='text-align: center;'>Value</th></tr></thead><tbody>";
        foreach($_SERVER as $key => $value) {
            if (array_key_exists($key, $json)) {
                if ($value == $json[$key]) {
                    echo "<tr style='background-color: #00FF00;'><td style='text-align: center;'>$key</td><td>$value</td style='text-align: center;'></tr>";
                } else {
                    echo "<tr style='background-color: #FF0000;'><td style='text-align: center;'>$key</td><td>$value</td style='text-align: center;'></tr>";
                }
            } else {
                echo "<tr style='background-color: #0000FF;'><td style='text-align: center;'>$key</td><td>$value</td style='text-align: center;'></tr>";
            }
        }
        echo "</tbody></table>";
    }
?>