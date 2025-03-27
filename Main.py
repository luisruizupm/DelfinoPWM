import tkinter as tk
from tkinter import ttk, messagebox
import serial
import serial.tools.list_ports
import time

class SerialConnectionApp(tk.Toplevel):
    def __init__(self, root):
        super().__init__(root)
        self.title("Serial Connection")
        self.configure(background="#1E1E1E")
        self.geometry("400x250")

        tk.Frame(self, height=25, bg="#54AEFF").pack(fill="x", pady=0)

        self.serial_settings = {}
        self.serial_connection = None
        
        self.create_widgets()
        self.refresh_ports()

    def create_widgets(self):
        frame = tk.Frame(self, bg="#1E1E1E")
        frame.pack(fill="both", expand=True, padx=10, pady=10)

        tk.Label(frame, text="Select COM Port:           ", bg="#1E1E1E", fg="#F8F8F2", font=("Cascade Mono", 11, "bold")).grid(row=0, column=0, sticky="w")
        self.port_combobox = ttk.Combobox(frame, width=15)
        self.port_combobox.grid(row=0, column=1, padx=5, pady=5, sticky="ew")

        tk.Label(frame, text="Baudrate:", bg="#1E1E1E", fg="#F8F8F2", font=("Cascade Mono", 11, "bold")).grid(row=1, column=0, sticky="w")
        self.baudrate_combobox = ttk.Combobox(frame, values=["9600", "14400", "19200", "38400", "57600", "115200"], width=15)
        self.baudrate_combobox.set("9600")
        self.baudrate_combobox.grid(row=1, column=1, padx=5, pady=5, sticky="ew")

        tk.Label(frame, text="Parity:", bg="#1E1E1E", fg="#F8F8F2", font=("Cascade Mono", 11, "bold")).grid(row=2, column=0, sticky="w")
        self.parity_combobox = ttk.Combobox(frame, values=["N", "E", "O", "M", "S"], width=15)
        self.parity_combobox.set("N")
        self.parity_combobox.grid(row=2, column=1, padx=5, pady=5, sticky="ew")

        tk.Label(frame, text="Data bits:", bg="#1E1E1E", fg="#F8F8F2", font=("Cascade Mono", 11, "bold")).grid(row=3, column=0, sticky="w")
        self.databits_combobox = ttk.Combobox(frame, values=[5, 6, 7, 8], width=15)
        self.databits_combobox.set(8)
        self.databits_combobox.grid(row=3, column=1, padx=5, pady=5, sticky="ew")

        tk.Label(frame, text="Stopbits:", bg="#1E1E1E", fg="#F8F8F2", font=("Cascade Mono", 11, "bold")).grid(row=4, column=0, sticky="w")
        self.stopbits_combobox = ttk.Combobox(frame, values=[1, 2], width=15)
        self.stopbits_combobox.set(1)
        self.stopbits_combobox.grid(row=4, column=1, padx=5, pady=5, sticky="ew")

        frame.columnconfigure(1, weight=1)

        button_frame = tk.Frame(self, bg="#1E1E1E")
        button_frame.pack(fill="x", padx=10, pady=10)

        self.save_button = tk.Button(button_frame, text="Connect", command=self.save_settings, bg="#54AEFF", fg="#F8F8F2", font=("Cascade Mono", 9, "bold"), width=8)
        self.save_button.pack(side="right", expand=False, padx=5)

        self.cancel_button = tk.Button(button_frame, text="Cancel", command=self.destroy, bg="#383c42", fg="#F8F8F2", font=("Cascade Mono", 9, "bold"), width=8)
        self.cancel_button.pack(side="right", expand=False, padx=5)

        self.refresh_button = tk.Button(button_frame, text="Refresh", command=self.refresh_ports, bg="#383c42", fg="#F8F8F2", font=("Cascade Mono", 9, "bold"), width=8)
        self.refresh_button.pack(side="right", expand=False, padx=5)

    def refresh_ports(self):
        ports = serial.tools.list_ports.comports()
        port_list = [port.device for port in ports]
        self.port_combobox['values'] = port_list
        if port_list:
            self.port_combobox.set(port_list[0])

    def save_settings(self):
        self.serial_settings["port"] = self.port_combobox.get()
        self.serial_settings["baudrate"] = self.baudrate_combobox.get()
        self.serial_settings["parity"] = self.parity_combobox.get()
        self.serial_settings["databits"] = self.databits_combobox.get()
        self.serial_settings["stopbits"] = self.stopbits_combobox.get()

        if not self.serial_settings["port"]:
            messagebox.showerror("Error", "Please select a COM port")
            return

        try:
            self.serial_connection = serial.Serial(
                port=self.serial_settings["port"],
                baudrate=int(self.serial_settings["baudrate"]),
                parity=self.serial_settings["parity"],
                stopbits=int(self.serial_settings["stopbits"]),
                bytesize=int(self.serial_settings["databits"]),
                timeout=1
            )
            # Pasar la conexión a la ventana principal
            self.master.serial_connection = self.serial_connection
            self.master.connection_status.config(text=f"Connected: {self.serial_settings['port']}", fg="green")
            messagebox.showinfo("Success", "Serial connection established successfully")
            self.destroy()
        except Exception as e:
            messagebox.showerror("Error", f"Failed to connect: {str(e)}")
            if self.serial_connection:
                self.serial_connection.close()

class MainApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("TI F28379D Serial Interface")
        self.geometry("500x400")
        self.configure(background="#1E1E1E")
        self.serial_connection = None  # Inicializar la conexión serial
        
        main_frame = tk.Frame(self, bg="#1E1E1E")
        main_frame.pack(fill="both", expand=True, padx=10, pady=10)
        
        self.create_controls(main_frame)
        self.protocol("WM_DELETE_WINDOW", self.on_close)

    def on_close(self):
        """Manejar el cierre de la aplicación"""
        if self.serial_connection and self.serial_connection.is_open:
            self.serial_connection.close()
        self.destroy()

    def set_frequency(self):
        """Establecer la frecuencia a través de la conexión serial en pasos de 5 Hz"""
        try:
            if not self.serial_connection or not self.serial_connection.is_open:
                messagebox.showerror("Error", "Serial connection is not open")
                return

            if not hasattr(self, "current_frequency"):
                self.current_frequency = 25  # Inicializar la frecuencia actual a 25 Hz

            target_frequency = float(self.frequency_var.get())
            if not (20 <= target_frequency <= 200):  # Validar rango
                messagebox.showerror("Error", "Frequency must be between 20-200 kHz")
                return

            step = 3 if target_frequency > self.current_frequency else -3

            while (step > 0 and self.current_frequency < target_frequency) or (step < 0 and self.current_frequency > target_frequency):
                self.current_frequency += step
                if (step > 0 and self.current_frequency > target_frequency) or (step < 0 and self.current_frequency < target_frequency):
                    self.current_frequency = target_frequency  # Ajustar el valor final

                T = round(1 / self.current_frequency * 5e4 - 1)
                self.serial_connection.write(f"PER:{T:04d}\n".encode())
                time.sleep(0.1)  # Pequeña pausa para simular la transición gradual

        except ValueError:
            messagebox.showerror("Error", "Please enter a valid number")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to set frequency: {str(e)}")

    def primary_bridge_update(self):
        """Actualizar los parámetros del puente primario"""
        try:
            if not self.serial_connection or not self.serial_connection.is_open:
                messagebox.showerror("Error", "Serial connection is not open")
                return

            duty_cycle = int(self.p_duty_cycle_var.get())
            dead_time1 = int(self.p_dead_time1_var.get())
            dead_time2 = int(self.p_dead_time2_var.get())

            PS = max(0, round(180 * 25 / 9 * (1 - duty_cycle / 50) - 1))

            dead_time1 = round(max(dead_time1, 20) / 20)
            dead_time2 = round(max(dead_time2, 20) / 20)

            self.serial_connection.write(f"PS:{PS:03d}\n".encode())
            print(f"Command sent: PS:{PS:03d}")
            time.sleep(0.1)
            self.serial_connection.write(f"DP1:{dead_time1:02d}\n".encode())
            print(f"Command sent: DP1:{dead_time1:02d}")
            time.sleep(0.1)
            self.serial_connection.write(f"DP2:{dead_time2:02d}\n".encode())
        except ValueError:
            messagebox.showerror("Error", "Please enter a valid number")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to update primary bridge: {str(e)}")

    def secondary_bridge_update(self):
        """Actualizar los parámetros del puente secundario"""
        try:
            if not self.serial_connection or not self.serial_connection.is_open:
                messagebox.showerror("Error", "Serial connection is not open")
                return

            phase_shift = int(self.s_phase_shift_var.get())
            dead_time1 = int(self.s_dead_time1_var.get())
            dead_time2 = int(self.s_dead_time2_var.get())

            phase_shift = max(0, round((499 + 1) * phase_shift / 90 - 1))

            dead_time1 = round(max(dead_time1, 20) / 20)
            dead_time2 = round(max(dead_time2, 20) / 20)

            self.serial_connection.write(f"SYNC:{phase_shift:03d}\n".encode())
            print(f"Command sent: SYNC:{phase_shift:04d}")
            time.sleep(0.1)
            self.serial_connection.write(f"DS1:{dead_time1:02d}\n".encode())
            print(f"Command sent: DS1:{dead_time1:02d}")
            time.sleep(0.1)
            self.serial_connection.write(f"DS2:{dead_time2:02d}\n".encode())
        except ValueError:
            messagebox.showerror("Error", "Please enter a valid number")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to update secondary bridge: {str(e)}")

    def create_controls(self, parent):
        title_frame = tk.Frame(parent, bg="#1E1E1E", pady=10)
        title_frame.pack(fill="x")
        tk.Label(title_frame, text="TI F28379D Serial Interface", bg="#1E1E1E", fg="white", font=("Arial", 16, "bold")).pack()

        # Estado de conexión
        self.connection_status = tk.Label(parent, text="Disconnected", fg="red", bg="#1E1E1E", font=("Arial", 10))
        self.connection_status.pack()

        freq_frame = tk.Frame(parent, bg="#1E1E1E", pady=10)
        freq_frame.pack(fill="x")
        tk.Label(freq_frame, text="Frequency (kHz):", bg="#1E1E1E", fg="white", font=("Arial", 12)).pack(side="left", padx=10)
        self.frequency_var = tk.StringVar(value="25")  # Valor inicial
        self.frequency_spinbox = tk.Spinbox(
            freq_frame, 
            from_=10, 
            to=200, 
            increment=1, 
            width=10, 
            textvariable=self.frequency_var
        )
        self.frequency_spinbox.pack(side="left", padx=5)
        set_freq_button = tk.Button(freq_frame, text="Set Frequency", bg="#54AEFF", fg="white", width=15, command=self.set_frequency)
        set_freq_button.pack(side="left", padx=10)

        bridge_frame = tk.Frame(parent, bg="#1E1E1E", pady=10)
        bridge_frame.pack(fill="both", expand=True)
        
        left_frame = tk.Frame(bridge_frame, bg="#252525", padx=10, pady=10)
        left_frame.pack(side="left", expand=True, fill="both", padx=5)
        
        right_frame = tk.Frame(bridge_frame, bg="#252525", padx=10, pady=10)
        right_frame.pack(side="right", expand=True, fill="both", padx=5)
        
        self.p_duty_cycle_var = tk.DoubleVar(value="50")
        self.p_dead_time1_var = tk.IntVar(value="100")
        self.p_dead_time2_var = tk.IntVar(value="100")
        self.s_phase_shift_var = tk.DoubleVar(value="90")
        self.s_dead_time1_var = tk.IntVar(value="100")
        self.s_dead_time2_var = tk.IntVar(value="100")

        self.add_spinbox_controls(left_frame, "Primary Bridge", [
            {"label": "Duty Cycle (%)", "from_": 0, "to": 50, "increment": 1, "initial": 50, "variable": self.p_duty_cycle_var},
            {"label": "Dead Time 1 (ns)", "from_": 20, "to": 500, "increment": 20, "initial": 100, "variable": self.p_dead_time1_var},
            {"label": "Dead Time 2 (ns)", "from_": 20, "to": 500, "increment": 20, "initial": 100, "variable": self.p_dead_time2_var}
        ])
        self.add_spinbox_controls(right_frame, "Secondary Bridge", [
            {"label": "Phase Shift", "from_": 0, "to": 180, "increment": 0.3, "initial": 90, "variable": self.s_phase_shift_var},
            {"label": "Dead Time 1", "from_": 20, "to": 500, "increment": 20, "initial": 100, "variable": self.s_dead_time1_var},
            {"label": "Dead Time 2", "from_": 20, "to": 500, "increment": 20, "initial": 100, "variable": self.s_dead_time2_var}
        ])

        self.enable_p_PWM_var = tk.BooleanVar(value=False)
        self.enable_s_PWM_var = tk.BooleanVar(value=False)

        enable_p_PWM = tk.Checkbutton(left_frame, text="Enable PWM", bg="#252525", fg="white",
                              selectcolor="#252525", variable=self.enable_p_PWM_var, 
                              command=self.toggle_primary_pwm)
        enable_p_PWM.pack(pady=5)
        
        enable_s_PWM = tk.Checkbutton(right_frame, text="Enable PWM", bg="#252525", fg="white",
                              selectcolor="#252525", variable=self.enable_s_PWM_var, 
                              command=self.toggle_secondary_pwm)
        enable_s_PWM.pack(pady=5)

        update_p_params_button = tk.Button(left_frame, text="Update Primary", bg="#54AEFF", fg="white", width=20, command=self.primary_bridge_update)
        update_p_params_button.pack(side="left", padx=10)

        update_s_params_button = tk.Button(right_frame, text="Update Secondary", bg="#54AEFF", fg="white", width=20, command=self.secondary_bridge_update)
        update_s_params_button.pack(side="right", padx=10)
        
        button_frame = tk.Frame(parent, bg="#1E1E1E", pady=10)
        button_frame.pack(fill="x")
        
        open_serial_button = tk.Button(button_frame, text="Open Serial Connection", command=self.open_serial, bg="#54AEFF", fg="white", width=20)
        open_serial_button.pack(side="right", padx=10)
    
    def toggle_primary_pwm(self):
        print(f"Primary PWM Enabled: {self.enable_p_PWM_var.get()}")
        if self.enable_p_PWM_var.get():
            self.serial_connection.write(f"pwmpon\n".encode())
        else:
            self.serial_connection.write(f"pwmpoff\n".encode())    
    def toggle_secondary_pwm(self):
        print(f"Secondary PWM Enabled: {self.enable_s_PWM_var.get()}")
        if self.enable_s_PWM_var.get():
            self.serial_connection.write(f"pwmson\n".encode())
        else:
            self.serial_connection.write(f"pwmsoff\n".encode())

    def add_spinbox_controls(self, parent, title, spinbox_configs):
        tk.Label(parent, text=title, bg="#252525", fg="white", font=("Arial", 12, "bold")).pack(pady=5)
        for config in spinbox_configs:
            row = tk.Frame(parent, bg="#252525")
            row.pack(fill="x", pady=2)
            tk.Label(row, text=f"{config['label']}:", bg="#252525", fg="white", width=15, anchor="w").pack(side="left")
            
            # Configuración adicional para el Spinbox
            spinbox = tk.Spinbox(
                row,
                from_=config["from_"],
                to=config["to"],
                increment=config["increment"],
                width=10,
                textvariable=config["variable"],  # Usamos la variable asociada
                validate="key",  # Validar entrada
                validatecommand=(row.register(self.validate_spinbox), '%P')
            )
            spinbox.pack(side="right", padx=5)

    def validate_spinbox(self, new_value):
        """Valida que la entrada sea numérica"""
        if new_value == "":
            return True
        try:
            float(new_value)
            return True
        except ValueError:
            return False
    
    def open_serial(self):
        SerialConnectionApp(self)

if __name__ == "__main__":
    app = MainApp()
    app.mainloop()