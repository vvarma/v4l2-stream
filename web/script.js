
async function fetchPipeline() {
  const response = await fetch('/pipeline');
  const data = await response.json();
  return data;
}

async function fetchControls(stage) {
    try {
        const response = await fetch(`/ctrls?dev_node=${stage.dev_node}`);
        const data = await response.json();
        return data; 
    } catch (error) {
        console.error('Error fetching controls:', error);
        return []; // Return an empty array in case of error
    }
}

function wrapP (element){
  const p = document.createElement("p");
  p.appendChild(element);
  return p;
}

function createSlider(control, dev_node) {
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
            dev_node: dev_node,
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

    sliderContainer.appendChild(wrapP(label));
    sliderContainer.appendChild(wrapP(slider));

    return sliderContainer;
}

async function startStream() {
    const imgElement = document.getElementById('stream-image');
    imgElement.src = '/stream';
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

    const sidenav = document.querySelector('.sidenav');
  
    fetchPipeline().then(pipeline => {
      pipeline.stages.forEach(stage => {
        console.log(stage);
        const stageDiv = document.createElement("div");
        stageDiv.className = "stage";
        const stageName = document.createElement("b");
        const newtext = document.createTextNode(stage.dev_node);
        stageName.appendChild(newtext);
        stageDiv.appendChild(stageName);
        sidenav.appendChild(stageDiv);
      
        fetchControls(stage).then(controls => {
          controls.forEach(control => {
            console.log(control);
            const slider = createSlider(control, stage.dev_node);
            stageDiv.appendChild(slider);
        });
      });
    });
  });
});

