async function fetchControls() {
    try {
        const response = await fetch('/ctrls');
        const data = await response.json();
        return data; 
    } catch (error) {
        console.error('Error fetching controls:', error);
        return []; // Return an empty array in case of error
    }
}

function createSlider(control) {
    const sliderContainer = document.createElement('div');
    sliderContainer.className = 'slider-container';

    const slider = document.createElement('input');
    slider.type = 'range';
    slider.min = control.min;
    slider.max = control.max;
    slider.value = control.current;
    slider.addEventListener('change', () => {
        // Handle slider value change
        const value = parseInt(slider.value);
        // Make a POST request to update control on server
        fetch('/ctrls', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
          body: JSON.stringify({
            controls:[
              {
                id: control.id,
                val: value
              }
            ]
          })
        });
    });

    const label = document.createElement('label');
    label.textContent = control.name;

    sliderContainer.appendChild(label);
    sliderContainer.appendChild(slider);

    return sliderContainer;
}

async function startStream() {
    const imgElement = document.getElementById('stream-image');
    imgElement.src = 'http://localhost:4891/stream';
}

function stopStream() {
    const imgElement = document.getElementById('stream-image');
    imgElement.src = ''; // Clear the image source to stop the stream
}

document.addEventListener('DOMContentLoaded', () => {
    const startButton = document.getElementById('start-stream');
    const stopButton = document.getElementById('stop-stream');

    startButton.addEventListener('click', startStream);
    stopButton.addEventListener('click', stopStream);
    fetchControls().then(controls => {
        const controlsContainer = document.querySelector('.controls');

        controls.forEach(control => {
      console.log(control);
            //if (control.type === 'int') {
                const slider = createSlider(control);
                controlsContainer.appendChild(slider);
            //}
        });
    });
});

