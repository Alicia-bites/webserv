<?php
    if ($_SERVER['SERVER_PORT'] == 80) { // nginx
        // encode $_SERVER as json
        echo json_encode($_SERVER);
    } else { // serveurtoile
        echo "<head><style>* {font-family: monospace;font-size: 16px;}h1 {font-size: 24px;text-align: center;}/* center table */table {margin-left: auto;margin-right: auto;}</style></head>";
        $content = file_get_contents(('http://localhost:8081/env_diff.php' . '?' . $_SERVER['QUERY_STRING']));
        $json = json_decode($content, true);
        $blacklisted_keys = ["USER", "HOME", "REQUEST_TIME_FLOAT", "REQUEST_TIME", "HTTP_HOST"];
        $warning_keys = ["SERVER_PORT", "SERVER_SOFTWARE", "REMOTE_ADDR", "SCRIPT_FILENAME", "SERVER_NAME"];
        // compare $_SERVER with json
        // values different = RED
        // values that are missing = BLUE
        // values that are the same = GREEN
        echo "<h1>CGI Metavariables</h1><hr>";
        echo "<table><thead><tr style='background-color: #3498db;'><th style='text-align: center;'>Variable</th><th style='text-align: center;'>serveurtoile</th><th style='text-align: center;'>nginx</th></tr></thead><tbody>";
        foreach($_SERVER as $key => $value) {
            if (array_key_exists($key, $json)) {
                // Skip if we have a blacklisted key
                if (in_array($key, $blacklisted_keys)) { continue; }
                if (in_array($key, $warning_keys)) {
                    echo "<tr style='background-color: #f1c40f; text: white;'><td style='padding: 2px; text-align: center;'>$key</td><td>$value</td style='text-align: center;'><td>$json[$key]</td></tr>";
                } else if ($value == $json[$key]) {
                    echo "<tr style='background-color: #2ecc71; text: white;'><td style='padding: 2px; text-align: center;'>$key</td><td>$value</td style='text-align: center;'><td>$json[$key]</td></tr>";
                } else {
                    echo "<tr style='background-color: #e74c3c; text: white;'><td style='padding: 2px; text-align: center;'>$key</td><td>$value</td style='text-align: center;'><td>$json[$key]</td></tr>";
                }
            }
        }
        echo "</tbody></table>";


        // Render all HTTP_* variables
        echo "<hr><h1>HTTP CGI Headers</h1><hr>";
        echo "<table><thead><tr><th style='text-align: center;'>Variable</th><th style='text-align: center;'>serveurtoile</th><th style='text-align: center;'>nginx</th></tr></thead><tbody>";
        foreach($_SERVER as $key => $value) {
            if (substr($key, 0, 5) == "HTTP_") {
                echo "<tr style='background-color: #ecf0f1; text: white;'><td style='padding: 2px; text-align: center;'>$key</td><td>$value</td style='text-align: center;'><td>$json[$key]</td></tr>";
            }
        }
        echo "</tbody></table>";

        // Show an explanation of the colors
        echo "<hr><h1>Explanation</h1><hr>";
        echo "<br>";
        echo '<div style="width: 50%; margin-left: auto; margin-right: auto; text-align: center;">';
        echo "<div class='e' style='background-color: #2ecc71; text: white; padding: 10px; margin: 10px; width: 100%;'>Green: The value is the same</div>";
        echo "<div class='e' style='background-color: #f1c40f; text: white; padding: 10px; margin: 10px; width: 100%;'>Yellow: The value is different, but it's not a problem</div>";
        echo "<div class='e' style='background-color: #e74c3c; text: white; padding: 10px; margin: 10px; width: 100%;'>Red: The value is different, and it's a problem</div>";
        echo '</div>';
    }
?>
