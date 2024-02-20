function deleteImage(fileName) {
	fetch(`../../images/png/delete/${fileName}.png`, {
		method: 'DELETE',
		headers: {
			'Content-Type': 'application/json',
		},
	})
	.then(response => {
		if (response.ok) {
			console.log(`DELETE request for ${fileName} successful`);
			document.getElementById(fileName).remove();
		} else {
			console.error(`DELETE request for ${fileName} failed`);
		}
	})
	.catch(error => {
		console.error('Error:', error);
	});
}
