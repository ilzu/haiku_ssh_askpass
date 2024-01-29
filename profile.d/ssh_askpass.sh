if [[ -z "${SSH_CLIENT}" ]]; then
	DISPLAY=HaikuDisplay
	export DISPLAY
fi

SSH_ASKPASS=/boot/system/bin/ssh_askpass
export SSH_ASKPASS

SSH_ASKPASS_REQUIRE=prefer
export SSH_ASKPASS_REQUIRE
