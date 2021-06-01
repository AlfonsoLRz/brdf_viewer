import brdf
import glm
import math
import random

if __name__ == '__main__':

    context = brdf.BRDF(brdf.Lambertian())
    print("Client: Strategy is set to normal sorting.")
    #context.compute_intensity()

    print("Client: Strategy is set to reverse sorting.")
    context.strategy = brdf.OrenNayar()
    #context.compute_intensity()

    horizontal_res = int(10 * 90.0)
    vertical_res = int(10 * 360.0)

    with open("tekari.txt", "w") as brdf_file:
        brdf_file.write("#sample_label \"iri_flake_paper\"\n")
        brdf_file.write("#sample_name \"Iridescent_flake_paper\"\n")
        brdf_file.write("#comment \"(null)\"\n")
        brdf_file.write("#datapoints_in_file " + str(horizontal_res * vertical_res) + "\n")
        brdf_file.write("#format: theta	phi	[uA]\n")

        for phi in range(0, horizontal_res):
            for theta in range(0, vertical_res):
                phi_angle = 90.0 * float(phi) / horizontal_res
                theta_angle = 360 * float(theta) / vertical_res

                cos_phi = math.cos(math.radians(phi_angle))
                sin_phi = math.sin(math.radians(theta_angle))

                intensity = 1.0 / math.pi
                alpha = math.acos(glm.clamp(math.radians(phi_angle) / math.pi, .0, 1.0))
                height = math.sin(alpha) / intensity

                brdf_file.write(str(phi_angle) + "\t" + str(theta_angle) + "\t" +
                                str(height) + "\n")
