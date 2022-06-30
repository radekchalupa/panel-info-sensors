#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

typedef struct _ThermalSensors
{
	float cpu;
	float cpu_cores[64];
	float acpi_sensors[16];
	unsigned int cpu_cores_count;
	unsigned int acpi_sensors_count;
} ThermailSensors;

int main(int argc, char** argv)
{
	ThermailSensors tsensors;
	int shortview = 0;
	int maxonly = 0;
	int opt;
	while ((opt = getopt(argc, argv, "sm")) != -1)
	{
		if ('s' == opt)
		{
			shortview = 1;
		}
		if ('m' == opt)
		{
			maxonly = 1;
		}
	}
	memset(&tsensors, 0, sizeof(ThermailSensors));
	char buffer[1024];
	FILE* p_file = popen("sensors", "r");
	if (NULL == p_file)
	{
		return errno;
	}
	float dval;
	const unsigned int section_cpu = 1;
	const unsigned int section_acpitz = 0;
	unsigned int section = 0;
	size_t index_acpitz = 0;
	size_t index_cpu = 0;
	size_t max_cpu_count = sizeof(tsensors.cpu_cores) / sizeof(float);
	size_t max_acpi_count = sizeof(tsensors.acpi_sensors) / sizeof(float);
	char pattern[64];
	while (!feof(p_file))
	{
		if (index_acpitz >= max_acpi_count)
			break;
		if (index_cpu >= max_cpu_count)
			break;
		if (fgets(buffer, sizeof(buffer), p_file) != NULL)
		{
			if (*buffer == '\n')
			{
				section = 0;
				continue;
			}
			if (0 == strncasecmp(buffer, "coretemp", 8))
			{
				section = section_cpu;
				continue;
			}
			if (0 == strncasecmp(buffer, "acpitz", 6))
			{
				section = section_acpitz;
				continue;
			}
			
			if (section_cpu == section)
			{
				if (sscanf(buffer, "Package id 0: %f", &dval) == 1)
				{
					tsensors.cpu = dval;
					continue;
				}
				sprintf(pattern, "Core %d: ", (int)index_cpu);
				strcat(pattern, "%f");
				if (sscanf(buffer, pattern, &dval) == 1)
				{
					tsensors.cpu_cores[index_cpu] = dval;
					index_cpu++;
					continue;
				}
			}
			else if (section_acpitz == section)
			{
				sprintf(pattern, "temp%d: ", (int)index_acpitz+1);
				strcat(pattern, "%f");
				if (sscanf(buffer, pattern, &dval) == 1)
				{
					tsensors.acpi_sensors[index_acpitz] = dval;
					index_acpitz++;
					continue;
				}
			}
		}
		else
			break;
	}
	tsensors.cpu_cores_count = index_cpu;
	tsensors.acpi_sensors_count = index_acpitz;
	pclose(p_file);
	float max_core = 0;
	for (size_t i = 0; i < tsensors.cpu_cores_count; i++)
	{
		if (tsensors.cpu_cores[i] > max_core)
			max_core = tsensors.cpu_cores[i];
	}
	if (maxonly)
	{
		if (shortview)
			printf("%3.1f -", tsensors.cpu > max_core ? tsensors.cpu : max_core);
		else
			printf("cpu: %3.1f - acpi:", tsensors.cpu > max_core ? tsensors.cpu : max_core);
		float max_acpi = 0.0;
		for (size_t i = 0; i < tsensors.acpi_sensors_count; i++)
		{
			if (tsensors.acpi_sensors[i] > max_acpi)
				max_acpi = tsensors.acpi_sensors[i];
		}
		printf(" %3.1f", max_acpi);
	}
	else
	{
		if (shortview)
			printf("%3.1f %3.1f -", tsensors.cpu, max_core);
		else
			printf("cpu: %3.1f %3.1f - acpi:", tsensors.cpu, max_core);
		for (size_t i = 0; i < tsensors.acpi_sensors_count; i++)
		{
			printf(" %3.1f", tsensors.acpi_sensors[i]);
		}
	}
	return EXIT_SUCCESS;
}
