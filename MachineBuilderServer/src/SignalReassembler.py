class SignalReassembler:
    def __init__(self):
        self.current_chunks = {}
        self.expected_total = None

    def reset(self):
        self.current_chunks = {}
        self.expected_total = None

    def add_fragment(self, message):
        """
        Procesa un fragmento tipo:
        <DATA>:id=2;total=5;payload=abc123;
        """
        if not message.startswith("<DATA>:"):
            return None  # formato no válido

        try:
            # Quitar <DATA>: y dividir en partes
            parts = message[len("<DATA>:"):].strip().strip(';').split(";")
            data = {kv.split("=")[0]: kv.split("=")[1] for kv in parts}

            chunk_id = int(data["id"])
            total_chunks = int(data["total"])
            payload = data["payload"]

            # Setear total si es la primera vez
            if self.expected_total is None:
                self.expected_total = total_chunks

            self.current_chunks[chunk_id] = payload

            # Verificamos si ya están todos los chunks
            if len(self.current_chunks) == self.expected_total:
                full_data = "".join(
                    self.current_chunks[i] for i in sorted(self.current_chunks)
                )
                self.reset()
                return full_data

        except Exception as e:
            print(f"⚠️ Error procesando fragmento: {e}")
            return None

        return None  # señal aún incompleta
