
#include <assert.h>

#include <haka/packet.h>
#include <haka/error.h>
#include <haka/packet_module.h>


static struct packet_module *packet_module = NULL;

int set_packet_module(struct module *module)
{
	struct packet_module *prev_packet_module = packet_module;

	if (module && module->type != MODULE_PACKET) {
		error(L"'%ls' is not a packet module", module->name);
		return 1;
	}

	if (module) {
		packet_module = (struct packet_module *)module;
		module_addref(&packet_module->module);
	}
	else {
		packet_module = NULL;
	}

	if (prev_packet_module) {
		module_release(&prev_packet_module->module);
	}

	return 0;
}

int has_packet_module()
{
	return packet_module != NULL;
}

struct packet_module *get_packet_module()
{
	return packet_module;
}

size_t packet_length(struct packet *pkt)
{
	assert(packet_module);
	assert(pkt);
	return packet_module->get_length(pkt);
}

const uint8 *packet_data(struct packet *pkt)
{
	assert(packet_module);
	assert(pkt);
	return packet_module->get_data(pkt);
}

uint8* packet_make_modifiable(struct packet *pkt)
{
	assert(packet_module);
	assert(pkt);
	return packet_module->make_modifiable(pkt);
}
