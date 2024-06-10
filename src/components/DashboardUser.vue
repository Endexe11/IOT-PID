<template>
  <main class="container">
    <div class="header">
      <h1 class="title">Laboratorio de control PID</h1>
      <button @click="logout" class="logout-btn">Logout</button>
    </div>
    <div class="dashboard-row">
      <div class="dashboard-column">
        <div class="dashboard-card" @click="startExperiment1">
          <div class="card-title">Iniciar Experimento 1</div>
        </div>
        <div class="dashboard-card" @click="startExperiment2">
          <div class="card-title">Iniciar Experimento 2</div>
        </div>
        <div class="dashboard-card" @click="startExperiment3">
          <div class="card-title">Iniciar Experimento 3</div>
        </div>
      <div class="dashboard-card" @click="Resultados">
        <div class="card-title">Resultados</div>
      </div>
    </div>
    </div>
    <div class="info-section">
      <h2>¿Cómo funciona un control PID?</h2>
      <p>Un controlador PID (Proporcional-Integral-Derivativo) es un mecanismo de control de bucle de retroalimentación ampliamente utilizado en sistemas de control industrial.</p>
      <h3>Componentes del Control PID:</h3>
      <ul>
        <li>
          <strong>Proporcional (<span v-html="renderMath('K_p')"></span>):</strong> 
          Determina la reacción al error actual. Un valor proporcional alto puede conducir a una respuesta rápida pero puede causar oscilaciones.
        </li>
        <li>
          <strong>Integral (<span v-html="renderMath('K_i')"></span>):</strong> 
          Determina la reacción basada en la suma de errores pasados. Ayuda a eliminar el error acumulado (offset) pero puede causar sobreoscilaciones si es muy alto.
        </li>
        <li>
          <strong>Derivativo (<span v-html="renderMath('K_d')"></span>):</strong> 
          Determina la reacción basada en la tasa de cambio del error. Ayuda a predecir y corregir futuros errores, proporcionando estabilidad al sistema.
        </li>
      </ul>
      <h3>¿Cómo ajustar <span v-html="renderMath('K_p')"></span>, <span v-html="renderMath('K_i')"></span>, y <span v-html="renderMath('K_d')"></span> utilizando el método de prueba y error?</h3>
      <p>El ajuste de los parámetros <span v-html="renderMath('K_p')"></span>, <span v-html="renderMath('K_i')"></span>, y <span v-html="renderMath('K_d')"></span> utilizando el método de prueba y error sigue los siguientes pasos:</p>
      <ol>
        <li>
          Establece <span v-html="renderMath('K_i')"></span> y <span v-html="renderMath('K_d')"></span> a cero.
        </li>
        <li>
          Incrementa <span v-html="renderMath('K_p')"></span> hasta que el sistema oscile continuamente.
        </li>
        <li>
          Ajusta <span v-html="renderMath('K_p')"></span> a aproximadamente la mitad de ese valor para obtener una respuesta amortiguada.
        </li>
        <li>
          Incrementa <span v-html="renderMath('K_i')"></span> hasta que cualquier error acumulado se corrija en un tiempo aceptable.
        </li>
        <li>
          Incrementa <span v-html="renderMath('K_d')"></span> si es necesario para reducir la oscilación.
        </li>
      </ol>
      <img src="../image/PID.gif" alt="PID_Control_GIF" class="pid-gif">
      <p>Las fórmulas que describen el comportamiento del PID son:</p>
      <ul>
        <li>
          <strong>Componente Proporcional:</strong> <span v-html="renderMath('P(t) = K_p \\cdot e(t)')"></span>
        </li>
        <li>
          <strong>Componente Integral:</strong> <span v-html="renderMath('I(t) = K_i \\cdot \\int e(t) \\, dt')"></span>
        </li>
        <li>
          <strong>Componente Derivativa:</strong> <span v-html="renderMath('D(t) = K_d \\cdot \\frac{de(t)}{dt}')"></span>
        </li>
      </ul>
      <p>La ecuación del PID se define como:</p>
      <p>
        <span v-html="renderMath('u(t) = K_p \\cdot e(t) + K_i \\cdot \\int e(t) \\, dt + K_d \\cdot \\frac{de(t)}{dt}')"></span>
      </p>
      <h3>Funcionamiento del laboratorio:</h3>
      <p>En este laboratorio, podrás iniciar un experimento de control PID, donde el sistema ajustará los parámetros <span v-html="renderMath('K_p')"></span>, <span v-html="renderMath('K_i')"></span>, y <span v-html="renderMath('K_d')"></span> para mantener la temperatura deseada. Los datos de temperatura actual y de referencia se mostrarán en tiempo real, permitiéndote observar cómo los cambios en los parámetros PID afectan al sistema.</p>
    </div>
  </main>
</template>

<script>
import { ref } from 'vue';
import { auth } from '@/firebaseConfig';
import { useRouter } from 'vue-router';
import katex from 'katex';
import 'katex/dist/katex.min.css';

export default {
  setup() {
    const router = useRouter();

    const logout = () => {
      auth.signOut();
      router.push('/');
    };

    const startExperiment1 = () => {
      router.push(`/experiment/1`);
    };
    const startExperiment2 = () => {
      router.push(`/experiment/2`);
    };
    const startExperiment3 = () => {
      router.push(`/experiment/3`);
    };
    const Resultados = () =>{
      router.push( `/comparison` )
    }

    const renderMath = (texString) => {
      return katex.renderToString(texString, {
        throwOnError: false
      });
    };

    return { logout, startExperiment1, startExperiment2, startExperiment3, Resultados, renderMath };
  }
};
</script>

<style scoped>
.container {
  max-width: 1200px;
  margin: 0 auto;
  padding: 20px;
}

.header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
}

.title {
  margin: 0;
}

.logout-btn {
  padding: 10px 20px;
  background-color: #007bff;
  color: #fff;
  border: none;
  border-radius: 5px;
  cursor: pointer;
}

.logout-btn:hover {
  background-color: #0056b3;
}

.dashboard-row {
  display: flex;
  justify-content: center;
}

.dashboard-column {
  flex: 1;
  max-width: 1200px;
  margin: 0 10px;
}

.dashboard-card {
  padding: 20px;
  border: 1px solid #ccc;
  border-radius: 5px;
  text-align: center;
  cursor: pointer;
  transition: box-shadow 0.3s ease-in-out;
}

.dashboard-card:hover {
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
}

.card-title {
  font-weight: bold;
  margin-bottom: 10px;
}

.card-description {
  color: #666;
}

.info-section {
  margin-top: 20px;
  text-align: left;
}

.info-section h2, .info-section h3 {
  margin-bottom: 10px;
}

.info-section p, .info-section ul, .info-section ol {
  text-align: left;
  margin-bottom: 10px;
}

.info-section ul, .info-section ol {
  padding-left: 20px;
}
</style>



