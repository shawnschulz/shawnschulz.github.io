import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { OrbitControls } from 'three/addons/controls/OrbitControls.js';
import { GUI } from 'dat.gui'
import * as THREE from 'three';

const scene = new THREE.Scene();
scene.background = new THREE.Color(0xf0f0f0);

// Camera setup
const camera = new THREE.PerspectiveCamera(75, window.innerWidth / 1.0 / window.innerHeight, 0.1, 1000);
camera.position.z = 5;

// Renderer setup
const renderer = new THREE.WebGLRenderer();
renderer.setSize(window.innerWidth, window.innerHeight);
const container = document.getElementById( 'canvas' );
document.body.appendChild(container);

// Lighting
const ambientLight = new THREE.AmbientLight(0xffffff, 0.5);
scene.add(ambientLight);

const directionalLight = new THREE.DirectionalLight(0xffffff, 0.8);
directionalLight.position.set(0, 1, 0);
scene.add(directionalLight);

// Orbit Controls
const controls = new OrbitControls( camera, renderer.domElement );

//controls.update() must be called after any manual changes to the camera's transform
camera.position.set( 0, 1, 10);
controls.update();

// GLTF Loader
const loader = new GLTFLoader();
let model = new THREE.Object3D();


// balgh
loader.load(
    './src/stromal_like.glb', (gltf) => {
        model = gltf.scene;
        model.name = 'stromal_cell';
        scene.add(model);
        model.position.set(0,0,0);
    }
);

//loader.load(
//    // Resource URL - replace with your GLTF file path
//    './data/cells/stromal_like.glb',
//    
//    // Called when the resource is loaded
//    function (gltf) {
//        scene.add(gltf.scene);
//    },
//    
//    // Called while loading is progressing
//    function (xhr) {
//        console.log((xhr.loaded / xhr.total * 100) + '% loaded');
//    },
//    
//    // Called when loading has errors
//    function (error) {
//        console.error('An error happened', error);
//    }
//);

// Make 3 different colored cubes for the demo
//const geometry = new THREE.BoxGeometry( 1, 1, 1 ); 
//const material = new THREE.MeshBasicMaterial( {color: 0x00ff00} ); 
//const cube = new THREE.Mesh( geometry, material ); 

function randrange(min, max) {
    return Math.floor(Math.random() * (max - min + 1)) + min;
}

function randfloat(min, max, precision=1000) {
}

// Generates a position randomly at the inputted distance away from
// 0,0,0. This is a uniform random sample with replacement, aka white noise
// placement. Works for a demo, but may want to do a blue noise or gaussian
// noise pattern since white noise distributions probably don't mimic 
// cellular protein spatial distributions. Could also add an option for
// some kind of ML inferred protein placement
function random_surface_placement(distance, min_distance=0, precision = 1000) {
    let ret = [0, 0, 0]
    let axes = [0, 1, 2]
    // Using squares cuz idk
    let remaining_distance_square = distance * distance;
    let sign = 1
    for (let i = 0; i <= 2; i++) {
        // Randomly choose x, y, or z 
        const index = Math.floor(Math.random() * axes.length)
        const axis = axes[index];
        // generate number between sqrt(0) and sqrt(remaining distance)
        const max = Math.floor(remaining_distance_square * precision)
        // This doesn't really make any sense for  if min_distance != 0 
        const square_component = randrange(min_distance, max) / precision
        // idk randomly 50/50 for negative or not (idk if this is technically mathematically distinct from a range with negatives)
        const negative = randrange(1, 100)
        if (negative < 51) {
            sign = -1
        }
        else {
            sign = 1
        }
        

        if (i < 2) {
            ret[axis] = Math.sqrt(square_component) * sign
            axes.splice(index, 1)
        }
        else {
            ret[axes[index]] = Math.sqrt(remaining_distance_square * sign)
            return ret
        }
        remaining_distance_square = remaining_distance_square - square_component
    }
    return ret
}

// TODO: we need a color mapping (and ideally also seperate shapes) for each surface receptor
function placeOnSurface(receptor_color=0x1E88E5, receptor_shape, protein_render_level=100, receptor_abundance_precent=0.5, receptor_name) {
    // Take a source object, get a random set of vertices, then place
    // the surface receptor gltf_object at the vertex
    // Can go with this for now, however if our render targets become more complex
    // may want to consider using threejs's InstancedMesh class instead of the default Mesh
    for (let i = 0; i < Math.floor(protein_render_level * receptor_abundance_precent); i++) {
        const geometry = new THREE.SphereGeometry( 0.5, 32, 16 ); 
        const material = new THREE.MeshBasicMaterial( {color: receptor_color} ); 
        const cube = new THREE.Mesh( geometry, material ); 
        scene.add( cube );
        const protein_position = random_surface_placement(2.806)
        cube.position.set(protein_position[0], protein_position[1], protein_position[2]);
        cube.scale.set(0.25,0.25,0.25);

    }
}

// Rendering loop
function animate() {
    requestAnimationFrame(animate);

	// required if controls.enableDamping or controls.autoRotate are set to true
	controls.update();

    renderer.render(scene, camera);
}
animate();
const gltf_object = scene.getObjectByName( "stromal_cell" );
let color2 = 0xFF0000;
const gui = new GUI();
const cellsFolder = gui.addFolder('Cells')
const inferenceFolder = gui.addFolder('Inference')
const colorFolder = gui.addFolder('Colors');
const receptorDensity = gui.addFolder('Receptor Rendering')
//colorFolder.add(color2, 'color')
placeOnSurface(gltf_object);
placeOnSurface(0xFFC107)
placeOnSurface(0x004D40)

function getProteins() {
    // Should do an API request to the backend to get the proteins
    // the user wants to add to the cell. This should create a mapping
    // of the protein name to a color and shape, we can save the mapping
    // to an SQL database and either in this or separate function 
    // go through all the mappings and create receptor objects and place
    // on surface
}

function getReceptorObject(color, shape) {
    // Generates the requested receptor mesh. 
    // Gonna start with reading the shape from a file, but
    // could change this to preload shapes
    // Color should either programatically create the material
    // or read the color material from a file
}


// Handle window resizing
window.addEventListener('resize', () => {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
});
