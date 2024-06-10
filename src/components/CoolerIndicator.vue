<template>
  <div :class="coolerClass" :style="coolerStyle"></div>
</template>

<script>
export default {
  name: 'CoolerIndicator',
  props: {
    pwm: {
      type: Number,
      required: true
    }
  },
  data() {
    return {
      animationName: 'spin'
    };
  },
  computed: {
    coolerClass() {
      //console.log('Cooler PWM:', this.pwm);
      return {
        'cooler': true,
        [this.animationName]: this.pwm !== 0
      };
    },
    coolerStyle() {
      return {
        animationDuration: this.pwm !== 0 ? `${1 + (1024 - this.pwm) / 1024}s` : '0s',
      };
    }
  },
  watch: {
    pwm(newVal) {
      if (newVal === 0) {
        this.animationName = ''; // Detener la animación
      } else {
        // Reiniciar la animación forzando la actualización del nombre de la animación
        this.animationName = 'none';
        this.$nextTick(() => {
          this.animationName = 'spin';
        });
      }
    }
  }
};
</script>

<style scoped>
.cooler {
  width: 100px;
  height: 100px;
  border-radius: 50%;
  background-color: gray;
  margin: 10px;
  background-image: url('/src/image/cooler.webp'); /* Actualiza con la ruta de tu imagen del cooler */
  background-size: cover;
}

.spin {
  animation: spin infinite linear;
}

@keyframes spin {
  0% {
    transform: rotate(0deg);
  }
  100% {
    transform: rotate(360deg);
  }
}
</style>



  