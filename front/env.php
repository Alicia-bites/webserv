<table>
	<thead>
		<tr>
			<th style="text-align: center;">Variable</th>
			<th style="text-align: center;">Value</th>
		</tr>
	</thead>
	<tbody>
		<?php
			foreach($_SERVER as $key => $value) {
				echo "<tr><td style='text-align: center;'>$key</td><td>$value</td style='text-align: center;'></tr>";
			}
		?>
	</tbody>
</table>