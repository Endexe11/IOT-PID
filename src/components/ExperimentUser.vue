<template>
  <div class="experiment-user">
    <h2>Iniciar Experimento {{ currentExperiment }}</h2>
    <div class="form-group">
      <label for="kp">Constante proporcional Kp:</label>
      <input type="text" id="kp" v-model="Kp" placeholder="Ingrese el valor de Kp" :disabled="experimentStarted">
    </div>
    <div class="form-group">
      <label for="ki">Constante integral Ki:</label>
      <input type="text" id="ki" v-model="Ki" placeholder="Ingrese el valor de Ki" :disabled="experimentStarted">
    </div>
    <div class="form-group">
      <label for="kd">Constante derivativa Kd:</label>
      <input type="text" id="kd" v-model="Kd" placeholder="Ingrese el valor de Kd" :disabled="experimentStarted">
    </div>
    <div class="form-group">
      <label for="tempRef">Temperatura de Referencia:</label>
      <input type="text" id="tempRef" v-model="tempRef" placeholder="Ingrese la temperatura de referencia" :disabled="experimentStarted">
    </div>
    <button @click="startExperiment" :disabled="experimentStarted">Iniciar Experimento</button>
    <button v-if="experimentStarted && currentExperiment < 3" @click="nextExperiment"  >Siguiente Experimento</button>
    <button v-if="currentExperiment === 3 && experimentStarted" @click="finishExperiments">Finalizar Experimentos</button>
    <div v-if="experimentStarted && experimentData" class="chart-container">
      <LineChart :experimentData="experimentData" :experimentId="currentExperiment"></LineChart>
    </div>
  </div>
</template>

<script>
import experimentService from "@/services/experimentService";
import { auth } from "@/firebaseConfig";
import LineChart from "@/components/LineChart.vue";

export default {
  components: {
    LineChart
  },
  data() {
    return {
      Kp: '',
      Ki: '',
      Kd: '',
      tempRef: '',
      experimentStarted: false,
      experimentData: {},
      currentExperiment: 1 // Iniciar con el experimento 1
    };
  },
  methods: {
    async startExperiment() {
      try {
        const uid = auth.currentUser?.uid;
        if (!uid) {
          throw new Error("Usuario no autenticado.");
        }

        // Convertir los valores a números
        const Kp = parseFloat(this.Kp);
        const Ki = parseFloat(this.Ki);
        const Kd = parseFloat(this.Kd);
        const temperaturaReferencia = parseFloat(this.tempRef);
        const started = this.experimentStarted;
        if (isNaN(Kp) || isNaN(Ki) || isNaN(Kd) || isNaN(temperaturaReferencia)) {
          throw new Error('Por favor, ingrese valores numéricos válidos.');
        }

        // Llamar al servicio para iniciar el experimento correspondiente
        await experimentService.setParametersToExperiment(
          { Kp, Ki, Kd, temperaturaReferencia, },
          uid,
          this.currentExperiment
        );

        await experimentService.setExperimentStarted(uid, this.currentExperiment, started);


        // Escuchar cambios en tiempo real de la base de datos para el experimento actual
        experimentService.getExperimentData(uid, this.currentExperiment, (data) => {
          if (data) {
            const timestamps = Object.keys(data).slice(-500);
            this.experimentData = timestamps.reduce((acc, timestamp) => {
              acc[timestamp] = data[timestamp];
              return acc;
            }, {});
          }
        });

        // Cambiar el estado de experimentStarted
        this.experimentStarted = true;
        alert(`El experimento ${this.currentExperiment} se inició correctamente.`);
      } catch (error) {
        console.error("Error al iniciar el experimento:", error);
        alert(`Ocurrió un error al iniciar el experimento: ${error.message}`);
      }
    },
    nextExperiment() {
      if (this.currentExperiment < 3) {
        this.currentExperiment++;
        this.experimentStarted = false; // Restablecer el formulario
        this.Kp = '';
        this.Ki = '';
        this.Kd = '';
        this.tempRef = '';
        this.experimentData = {}; // Reiniciar los datos del experimento
      }
    },
    finishExperiments() {
      // Redirigir al usuario a la página de comparación
      this.$router.push("/comparison");
    }
  }
};
</script>

<style scoped>
.experiment-user {
  max-width: 800px;
  margin: 0 auto;
  padding: 20px;
  border: 1px solid #ccc;
  border-radius: 5px;
}

.form-group {
  margin-bottom: 15px;
}

label {
  font-weight: bold;
}

input[type="text"] {
  padding: 10px;
  width: 100%;
  border: 1px solid #ccc;
  border-radius: 5px;
}

input[type="text"]:disabled {
  background-color: #e9ecef;
}

button {
  padding: 10px;
  background-color: #007bff;
  color: #fff;
  border: none;
  border-radius: 5px;
  cursor: pointer;
}

button:disabled {
  background-color: #cccccc;
  cursor: not-allowed;
}

button:hover:enabled {
  background-color: #0056b3;
}

.chart-container {
  margin-top: 20px;
}
</style>





