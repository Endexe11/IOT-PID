<template>
  <div class="experiment-container">
    <h2>Comparación de Experimentos</h2>
    <br>
    <button @click="goToDashboard" class="back-button">Inicio</button>
    <div v-for="experimentId in [1, 2, 3]" :key="experimentId" class="experiment">
      <h3>Experimento {{ experimentId }}</h3>
      <div class="canvas-container">
        <canvas :id="'experimentCanvas' + experimentId" class="chart-canvas" :data-id="experimentId"></canvas>
      </div>
      <img :src="getExperimentImage(experimentId)" alt="Experiment Image" class="experiment-image" :data-id="experimentId" @load="adjustCanvasSize">
    </div>
  </div>
</template>

<script>
import { ref, onValue } from "firebase/database";
import { database } from "@/firebaseConfig";
import { auth } from "@/firebaseConfig";
import router from "@/router"; // Importamos el enrutador

export default {
  data() {
    return {
      experimentImages: {
        experiment1: "", // URLs para cada experimento
        experiment2: "",
        experiment3: ""
      }
    };
  },
  mounted() {
    [1, 2, 3].forEach(experimentId => {
      this.loadExperimentImage(experimentId); // Cargar URL de la imagen para cada experimento
    });
  },
  methods: {
    loadExperimentImage(experimentId) {
      const uid = auth.currentUser.uid;
      const imageRef = ref(database, `UsersData/${uid}/Resultados_Experimentos/Experimento_${experimentId}/imageUrl`);

      onValue(imageRef, (snapshot) => {
        const data = snapshot.val();
        if (data) {
          // Actualizar la URL de la imagen para el experimento correspondiente
          this.experimentImages[`experiment${experimentId}`] = data;
        }
      });
    },
    getExperimentImage(experimentId) {
      return this.experimentImages[`experiment${experimentId}`];
    },
    adjustCanvasSize(event) {
      const experimentId = event.target.dataset.id;
      const canvas = document.getElementById(`experimentCanvas${experimentId}`);
      const img = event.target;
      canvas.width = img.width;
      canvas.height = img.height;
    },
    goToDashboard() {
      // Redirigir al usuario al Dashboard
      router.push("/dashboard");
    }
  },
};
</script>

<style scoped>
button {
  padding: 20px;
  background-color: #ff5f5f;
  color: #fff;
  border: none;
  border-radius: 8px;
  cursor: pointer;
}
.experiment-container {
  max-width: 1000px;
  margin: 0 auto;
}

.experiment {
  margin-bottom: 60px;
  position: relative; 
}

.canvas-container {
  width: 100%;
  max-width: 600px;
  overflow: hidden; 
  position: absolute; 
}

.chart-canvas {
  width: 100%;
  height: auto; 
  max-height: 300px; 
}

.experiment-image {
  position: relative; 
  top: 0;
  left: 0;
  width: 100%; 
  height: auto;
}

.back-button {
  margin-top: 20px; 
}
</style>













  