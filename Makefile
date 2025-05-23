# Répertoires
SRC_DIR := srcs
OBJ_DIR := objs
INC_DIR := includes

# Variables de compilation
CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -g3 -std=c++98 -I$(INC_DIR) -I./srcs/cmds

# Fichiers sources et objets
SRCS := $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(SRCS:.cpp=.o))
DEPS := $(OBJS:.o=.d)

# Nom de l'exécutable
NAME := irc

# Nombre total de fichiers
TOTAL_FILES := $(words $(SRCS))

# Compteur de fichiers actuellement compilés
CURRENT_FILE := 0

# Fonction pour mettre à jour et afficher la barre de progression sur une seule ligne
define update_progress
		$(eval CURRENT_FILE=$(shell echo $$(($(CURRENT_FILE)+1))))
		$(eval PERCENT=$(shell echo $$((($(CURRENT_FILE)*100)/$(TOTAL_FILES)))))
		@printf "\rmake ircserv : ["
		@printf "%-50s" "$$(printf '#%.0s' $$(seq 1 $$(($(PERCENT)/2))))"
		@printf "] %d%%" $(PERCENT)
endef

# Cible par défaut
all: $(NAME)

# Règle de liaison
$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) -o $@ $^
# Compilation des fichiers objets avec gestion des dépendances
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@
	@$(call update_progress)

# Inclusion des fichiers de dépendances
-include $(DEPS)

# Fonction pour la barre de progression de clean
define clean_progress
		$(eval TOTAL_COUNT=$(words $(OBJS) $(DEPS) 1)) # +1 pour le répertoire objs
		$(eval CURRENT_COUNT=$(shell echo $$(($(CURRENT_COUNT)+1))))
		$(eval PERCENT=$(shell echo $$((($(CURRENT_COUNT)*100)/$(TOTAL_COUNT)))))
		@printf "\rclean ircserv : ["
		@printf "%-50s" "$$(printf '#%.0s' $$(seq 1 $$(($(PERCENT)/2))))"
		@printf "] %d%%" $(PERCENT)
endef

# Fonction pour la barre de progression de fclean
define fclean_progress
		$(eval TOTAL_COUNT=1) # Seulement pour l'exécutable ircserv
		$(eval CURRENT_COUNT=$(shell echo $$(($(CURRENT_COUNT)+1))))
		$(eval PERCENT=$(shell echo $$((($(CURRENT_COUNT)*100)/$(TOTAL_COUNT)))))
		@printf "\rfclean ircserv : ["
		@printf "%-50s" "$$(printf '#%.0s' $$(seq 1 $$(($(PERCENT)/2))))"
		@printf "] %d%%" $(PERCENT)
endef

# Cibles de nettoyage
clean:
	@$(eval CURRENT_COUNT=0)
	@$(foreach obj,$(OBJS) $(DEPS), rm -f $(obj); $(call clean_progress);)
	@rm -rf $(OBJ_DIR)
	@$(call clean_progress) # Pour afficher 100%
fclean:
	@$(eval CURRENT_COUNT=0)
	@$(MAKE) clean > /dev/null
	@rm -f $(NAME)
	@$(call fclean_progress) # Pour afficher 100%
re: fclean all

# Indication des cibles "phony"
.PHONY: all clean fclean re update_progress